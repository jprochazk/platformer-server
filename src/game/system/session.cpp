
#include "game/system/session.h"
#include "common/assert.h"
#include "common/enum_util.h"
#include "common/log.h"
#include "game/event.h"
#include "game/world.h"
#include "network/opcode.h"
#include <iostream>
#include <map>
#include <set>

namespace game {
namespace system {

session::session(std::shared_ptr<game::world> world,
                 std::shared_ptr<network::packet::handler_table> handler_table)
  : connection_counter_(0)
  , connection_queue_()
  , message_counter_(0)
  , message_queue_()
  , disconnection_counter_(0)
  , disconnection_queue_()
  , handler_table_(std::move(handler_table))
  , world(world)
{
    INFO("SESSION", "Initialized");
}

void
session::update()
{
    // INFO("SESSION", "Update");
    handle_connections();
    handle_messages();
    handle_disconnections();
}

void
session::on_open(uint32_t id, std::weak_ptr<network::socket_base> socket)
{
    connection_queue_.enqueue(component::session{ id, std::move(socket), {} });
    connection_counter_.fetch_add(1, std::memory_order_release);
}

void
session::on_close(uint32_t id)
{
    disconnection_queue_.enqueue(id);
    disconnection_counter_.fetch_add(1, std::memory_order_release);
}

void
session::on_message(uint32_t id, std::vector<uint8_t>&& data)
{
    message_queue_.enqueue(message{ id, std::move(data) });
    message_counter_.fetch_add(1, std::memory_order_release);
}

void
session::on_error(uint32_t id, std::string_view what, beast::error_code error)
{
    if (error == net::error::operation_aborted ||
        error == net::error::connection_aborted ||
        error == beast::websocket::error::closed)
        return;

    ERRF("SOCKET", "ID {} -> [{}] ERROR: {}", id, what, error.message());
}

void
session::handle_connections()
{
    auto num_connections = connection_counter_.load(std::memory_order_acquire);
    connection_counter_.fetch_add(-static_cast<int64_t>(num_connections),
                                  std::memory_order_release);

    if (num_connections == 0)
        return;

    std::vector<component::session> sessions(num_connections,
                                             component::session{});
    size_t received =
      connection_queue_.try_dequeue_bulk(sessions.begin(), num_connections);
    debug_assert(received == num_connections,
                 "received != num_connections: {} != {}",
                 received,
                 num_connections);

    auto& registry = world->get_registry();
    std::vector<entt::entity> entities(num_connections);
    registry.create(entities.begin(), entities.end());

    debug_assert(sessions.size() == entities.size(),
                 "sessions.size != entities.size: {} != {}",
                 sessions.size(),
                 entities.size());

    auto& dispatcher = world->get_dispatcher();
    auto& map = world->get_map();
    for (size_t i = 0; i < num_connections; i++) {
        std::shared_ptr<network::socket_base> sock = sessions[i].socket.lock();
        if (!sock)
            return;

        INFOF("SESSION", "ID {} joined", sessions[i].id);
        // TODO: move this somewhere
        registry.emplace<component::session>(entities[i],
                                             std::move(sessions[i]));
        registry.emplace<component::input>(
          entities[i], component::input{ false, false, false, false });
        registry.emplace<component::body>(
          entities[i],
          component::body{ component::body::position{ 0, 0 },
                           component::body::position{ 0, 0 },
                           component::body::velocity{ 0, 0 },
                           0.9,  // gravity
                           10.0, // speed
                           18.0, // jump_velocity
                           component::body::mtype::WALK,
                           component::body::jump_state::NONE });
        registry.emplace<component::collider>(
          entities[i],
          component::collider{ collision::AABB{ collision::Vec{ 32, 32 },
                                                collision::Vec{ 32, 32 } } });
        dispatcher.publish<event::entity_join>(entities[i]);

        // TODO: persist zone id on session disconnect
        map.insert(entities[i], static_cast<zone::id>(0));

        sock->send(network::packet::serialize(
          common::to_underlying(network::server::opcode::ID),
          network::packet::server::id{ static_cast<uint32_t>(entities[i]) }));
    }
}

void
session::handle_messages()
{
    auto num_messages = message_counter_.load(std::memory_order_acquire);
    message_counter_.fetch_add(-static_cast<int64_t>(num_messages),
                               std::memory_order_release);

    if (num_messages == 0)
        return;

    std::vector<message> vmessages(num_messages, message{});
    message_queue_.try_dequeue_bulk(vmessages.begin(), num_messages);

    // turn vector into multimap for efficient lookup
    std::multimap<uint32_t, std::vector<uint8_t>> mmessages;
    for (auto& m : vmessages) {
        mmessages.insert(std::make_pair(m.id, std::move(m.data)));
    }
    vmessages.erase(vmessages.begin(), vmessages.end());

    auto& registry = world->get_registry();
    registry.view<component::session>().each([&](const entt::entity& entity,
                                                 component::session& session) {
        const auto [start, end] = mmessages.equal_range(session.id);
        for (auto it = start; it != end; it++) {
            auto& [id, buffer] = *it;

            network::packet::base packet = network::packet::deserialize(buffer);
            if (!packet.opcode)
                continue;

            auto handler = handler_table_->get(*packet.opcode);
            if (!packet.data)
                return;
            handler(*world, entity, *(packet.data));
        }
    });
}

void
session::handle_disconnections()
{
    auto num_disconnections =
      disconnection_counter_.load(std::memory_order_acquire);
    disconnection_counter_.fetch_add(-static_cast<int64_t>(num_disconnections),
                                     std::memory_order_release);

    if (num_disconnections == 0)
        return;

    std::vector<uint32_t> vdisconnections(num_disconnections, 0);
    disconnection_queue_.try_dequeue_bulk(vdisconnections.begin(),
                                          num_disconnections);

    std::set<uint32_t> sdisconnections;
    for (auto& dc : vdisconnections) {
        sdisconnections.insert(dc);
    }

    auto& registry = world->get_registry();
    auto& dispatcher = world->get_dispatcher();
    auto& map = world->get_map();
    registry.view<component::session>().each([&](const entt::entity& e,
                                                 component::session& s) {
        if (auto it = sdisconnections.find(s.id); it != sdisconnections.end()) {
            INFOF("SESSION", "Session ID {} left", s.id);

            // TODO: move this somewhere
            // TODO: persist zone id on session disconnect
            map.erase(e);
            registry.destroy(e);
            dispatcher.publish<event::entity_leave>(e);
        }
    });
}

} // namespace system
} // namespace game