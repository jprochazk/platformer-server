#pragma once

#include "game/system/system_base.h"
#include "game/world.h"
#include <gtest/gtest.h>

class test_system : public game::system_base
{
  public:
    size_t updateCalled = 0;

    void update() override { ++updateCalled; }
};

class test_system2 : public game::system_base
{
  public:
    void update() override {}
};

TEST(world, update)
{
    game::world world;

    auto system = std::make_shared<test_system>();
    world.add_system("test", system);
    world.update();

    EXPECT_EQ(system->updateCalled, 1);
}

TEST(world, add_system_duplicate)
{
    auto system = std::make_shared<test_system>();

    game::world world;
    world.add_system("test", system);
    try {
        world.add_system("test", system);
        FAIL();
    } catch (std::exception e) {
        std::string actual{ e.what() };
        std::string expected{ "System \"test\" already exists" };
        EXPECT_EQ(actual, expected);
    }
}

TEST(world, get_system)
{
    game::world world;
    world.add_system("test", std::make_shared<test_system>());

    auto actual = world.get_system<test_system>("test");
    EXPECT_TRUE(static_cast<bool>(actual));
}

TEST(world, get_system_unknown)
{
    game::world world;

    try {
        auto system = world.get_system<test_system>("test");
        FAIL();
    } catch (std::exception e) {
        std::string actual{ e.what() };
        std::string expected{ "System \"test\" does not exist" };
        EXPECT_EQ(actual, expected);
    }
}

TEST(world, get_system_wrong_type)
{
    game::world world;
    world.add_system("test", std::make_shared<test_system>());

    try {
        auto system = world.get_system<test_system2>("test");
        FAIL();
    } catch (std::exception e) {
        std::string actual{ e.what() };
        std::string expected{
            "System \"test\" is not of type \"class test_system2\""
        };
        EXPECT_EQ(actual, expected);
    }
}

TEST(world, remove_system)
{
    game::world world;

    auto system = std::make_shared<test_system>();
    world.add_system("test", system);
    world.remove_system("test");
    world.update();

    EXPECT_EQ(system->updateCalled, 0);
}