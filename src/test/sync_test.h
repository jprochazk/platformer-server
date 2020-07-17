#pragma once

#include "game/system/sync.h"
#include "game/system/zone.h"
#include "network/packet.h"
#include <gtest/gtest.h>

namespace sync_test {
class socket
  : public network::socket_base
  , public std::enable_shared_from_this<socket>
{
  public:
    bool closed_ = false;
    std::vector<std::vector<uint8_t>> messages;

    void open() override { closed_ = false; }
    void close() override { closed_ = true; }
    void send(std::vector<uint8_t> data) override { messages.push_back(data); }
    bool closed() const override { return closed_; }
};
} // namespace sync_test

TEST(sync, update)
{
    using namespace game;
    using namespace network;

    auto socket = std::make_shared<sync_test::socket>();

    auto world = std::make_shared<game::world>();

    world->add_system("zone", std::make_shared<system::zone>(world));
    world->add_system("sync", std::make_shared<system::sync>(world));

    auto& registry = world->get_registry();
    auto entity = registry.create();
    registry.emplace<component::session>(entity,
                                         component::session{ 0, socket });
    world->get_dispatcher().publish<event::entity_join>(entity);

    world->update();

    // socket should've received one packet
    EXPECT_EQ(socket->messages.size(), static_cast<size_t>(1));
    auto& message = socket->messages.front();
    auto packet = packet::deserialize(message);
    EXPECT_TRUE(packet.opcode.has_value());
    EXPECT_TRUE(packet.data.has_value());
    EXPECT_EQ(*packet.opcode, common::to_underlying(server::opcode::STATE));

    packet::server::state state = *packet.data;
    EXPECT_EQ(state.entities.size(), static_cast<size_t>(1));
    auto received_entity = state.entities[0];
    EXPECT_EQ(received_entity.id, static_cast<uint32_t>(0));
    EXPECT_EQ(received_entity.components->position->x, 0.f);
    EXPECT_EQ(received_entity.components->position->y, 0.f);
}