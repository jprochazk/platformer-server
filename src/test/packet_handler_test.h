#pragma once

#include "common/enum_util.h"
#include "game/component.h"
#include "game/world.h"
#include "network/handler.h"
#include "network/opcode.h"
#include <gtest/gtest.h>

TEST(packet_handler_test, move_input)
{
    using namespace network::packet;
    using namespace game::component;

    game::world world;
    auto& registry = world.get_registry();

    auto entity = registry.create();
    registry.emplace<velocity>(entity);

    default_handler_table table;

    auto handler = table.get(common::to_underlying(network::client::opcode::INPUT));

    auto vel_case = [&](uint8_t input, float e_x, float e_y) {
        json j = { { "f", input } };
        handler(world, entity, j);
        auto& vel = registry.get<velocity>(entity);
        EXPECT_EQ(vel.y, e_y);
        EXPECT_EQ(vel.x, e_x);
        vel.x = 0.f;
        vel.y = 0.f;
    };

    vel_case(1, 0, -1);
    vel_case(2, 0, 1);
    vel_case(3, 0, 0);
    vel_case(4, -1, 0);
    vel_case(5, -1, -1);
    vel_case(6, -1, 1);
    vel_case(7, -1, 0);
    vel_case(8, 1, 0);
    vel_case(9, 1, -1);
    vel_case(10, 1, 1);
    vel_case(11, 1, 0);
    vel_case(12, 0, 0);
}
