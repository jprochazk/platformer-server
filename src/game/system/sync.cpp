
#include "game/system/sync.h"
#include "common/enum_util.h"
#include "common/log.h"
#include "game/world.h"
#include "network/opcode.h"
#include "network/packet.h"
#include <cstddef>
#include <cstdint>
#include <map>
#include <vector>

namespace game {

network::packet::server::component_storage
get_components(const game::registry& registry, const entt::entity& entity)
{
    network::packet::server::component_storage storage{};
    auto body = registry.try_get<component::body>(entity);
    if (body) {
        storage.pos = { body->pos.x, body->pos.y };
    }
    return storage;
}

network::packet::server::state
serialize_state(const game::registry& registry,
                const std::unordered_set<entt::entity>& entities)
{
    network::packet::server::state state{};
    state.entities.reserve(entities.size());
    for (const auto& entity : entities) {
        state.entities.push_back(network::packet::server::entity{
          static_cast<uint32_t>(entity), get_components(registry, entity) });
    }
    return state;
}

namespace system {
sync::sync(std::shared_ptr<game::world> world)
  : world(world)
{
    INFO("SYNC", "Initialized");
}

void
sync::update()
{
    // INFO("SYNC", "Update");
    using namespace network;

    auto& registry = world->get_registry();
    auto& map = world->get_map();

    registry.view<component::session>().each(
      [&](const entt::entity& e, component::session& session) {
          std::shared_ptr<socket_base> sock = session.socket.lock();
          if (!sock)
              return;

          // TEMP many small allocations here
          sock->send(packet::serialize(
            common::to_underlying(server::opcode::STATE),
            serialize_state(registry, map.get_zone(e).get_entities())));
      });
}

} // namespace system
} // namespace game