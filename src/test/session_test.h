#pragma once

#include "common/enum_util.h"
#include "game/component.h"
#include "game/system/session.h"
#include "game/world.h"
#include "network/handler.h"
#include "network/opcode.h"
#include "network/packet.h"
#include <gtest/gtest.h>
#include <optional>

namespace session_test {

class handler_table : public network::packet::handler_table
{
  public:
    size_t requests = 0;

    network::packet::handler handler = { (uint16_t)-1, nullptr };
    network::packet::handler const& get(uint16_t opcode) override
    {
        ++requests;
        return handler;
    }
};

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

} // namespace session_test

TEST(session, connection)
{
    using namespace game;
    using namespace network;

    auto table = std::make_shared<session_test::handler_table>();
    auto socket = std::make_shared<session_test::socket>();

    auto world = std::make_shared<game::world>();
    auto system = std::make_shared<system::session>(world, table);
    world->add_system("session", system);

    system->on_open(0, socket->weak_from_this());

    world->update();

    auto& registry = world->get_registry();
    // registry should contain one entity with component session
    size_t entity_count = 0;
    registry.view<component::session>().each(
      [&](const entt::entity&, component::session&) { ++entity_count; });
    EXPECT_EQ(entity_count, static_cast<size_t>(1));

    // we should have also received a packet
    EXPECT_EQ(socket->messages.size(), static_cast<size_t>(1));
    auto& message = socket->messages.front();
    auto packet = packet::deserialize(message);
    EXPECT_TRUE(packet.opcode.has_value());
    EXPECT_EQ(*packet.opcode, common::to_underlying(server::opcode::ID));
    EXPECT_TRUE(packet.data.has_value());
    packet::server::id id = *packet.data;
    EXPECT_EQ(id.value, static_cast<uint32_t>(0));
}

TEST(session, disconnection)
{
    using namespace game;

    auto table = std::make_shared<session_test::handler_table>();
    auto socket = std::make_shared<session_test::socket>();

    auto world = std::make_shared<game::world>();
    auto system = std::make_shared<system::session>(world, table);
    world->add_system("session", system);

    system->on_open(0, socket->weak_from_this());
    system->on_close(0);

    world->update();

    auto& registry = world->get_registry();
    // registry should contain no entities
    size_t entity_count = 0;
    registry.view<component::session>().each(
      [&](const entt::entity&, component::session&) { ++entity_count; });
    EXPECT_EQ(entity_count, static_cast<size_t>(0));
}

TEST(session, message)
{
    using namespace game;
    using namespace network;

    auto table = std::make_shared<session_test::handler_table>();
    auto socket = std::make_shared<session_test::socket>();

    auto world = std::make_shared<game::world>();
    auto system = std::make_shared<system::session>(world, table);
    world->add_system("session", system);

    system->on_open(0, socket->weak_from_this());
    system->on_message(0, packet::serialize(static_cast<uint16_t>(-1), json{}));

    world->update();

    auto& registry = world->get_registry();
    // registry should contain one entity with component session
    size_t entity_count = 0;
    registry.view<component::session>().each(
      [&](const entt::entity&, component::session&) { ++entity_count; });
    EXPECT_EQ(entity_count, static_cast<size_t>(1));

    // handler_table should've had get() called once
    EXPECT_EQ(table->requests, static_cast<size_t>(1));
}