#pragma once

#include "game/system/zone.h"
#include "game/component.h"
#include <gtest/gtest.h>

TEST(zone, on_entity_join) {
	using namespace game;

	auto world = std::make_shared<game::world>();
	system::zone system{ world };

	auto& registry = world->get_registry();
	auto entity = registry.create();
	world->get_dispatcher().publish<event::entity_join>(entity);
	{
		auto found = system.query(0);
		EXPECT_EQ(found.size(), static_cast<size_t>(1));
	}

	auto expected = component::zoned{ 0, 0 };
	auto* actual = registry.try_get<component::zoned>(entity);
	EXPECT_TRUE(static_cast<bool>(actual));
	EXPECT_EQ(expected.zone, actual->zone);
	EXPECT_EQ(expected.layer, actual->layer);
}

TEST(zone, on_entity_leave) {
	using namespace game;

	auto world = std::make_shared<game::world>();
	system::zone system{ world };

	auto& registry = world->get_registry();
	auto entity = registry.create();

	auto& dispatcher = world->get_dispatcher();
	dispatcher.publish<event::entity_join>(entity);
	{
		auto found = system.query(0);
		EXPECT_EQ(found.size(), static_cast<size_t>(1));
	}

	auto expected = component::zoned{ 0, 0 };
	auto* actual = registry.try_get<component::zoned>(entity);
	EXPECT_TRUE(static_cast<bool>(actual));
	EXPECT_EQ(expected.zone, actual->zone);
	EXPECT_EQ(expected.layer, actual->layer);

	dispatcher.publish<event::entity_leave>(entity);
	{
		auto found = system.query(0);
		EXPECT_EQ(found.size(), static_cast<size_t>(0));
	}
}

TEST(zone, on_zone_transfer) {
	using namespace game;

	auto world = std::make_shared<game::world>();
	system::zone system{ world };

	auto& registry = world->get_registry();
	auto entity = registry.create();

	auto& dispatcher = world->get_dispatcher();
	dispatcher.publish<event::entity_join>(entity);
	system.update();
	{
		auto found = system.query(0);
		EXPECT_EQ(found.size(), static_cast<size_t>(1));
	}

	{
		auto expected = component::zoned{ 0, 0 };
		auto* actual = registry.try_get<component::zoned>(entity);
		EXPECT_TRUE(static_cast<bool>(actual));
		EXPECT_EQ(expected.zone, actual->zone);
		EXPECT_EQ(expected.layer, actual->layer);
	}

	dispatcher.publish<event::zone_transfer>(entity, static_cast<uint16_t>(1));
	system.update();
	{
		auto found = system.query((system::zone::zone_id)0);
		EXPECT_EQ(found.size(), static_cast<size_t>(0));
	}
	{
		auto found = system.query((system::zone::zone_id)1);
		EXPECT_EQ(found.size(), static_cast<size_t>(1));
	}

	{
		auto expected = component::zoned{ 1, 0 };
		auto* actual = registry.try_get<component::zoned>(entity);
		EXPECT_TRUE(static_cast<bool>(actual));
		EXPECT_EQ(expected.zone, actual->zone);
		EXPECT_EQ(expected.layer, actual->layer);
	}
}