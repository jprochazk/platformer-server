
#include "map.h"
#include "common/log.h"
#include <limits>

namespace game {
namespace collision {
	void from_json(const json& json, AABB& aabb) {
		aabb.center = Vec{
			json["center"]["x"].get<double>(),
			json["center"]["y"].get<double>()
		};
		aabb.half = Vec{
			json["half"]["x"].get<double>(),
			json["half"]["y"].get<double>()
		};
	}
}
}

namespace game {
	zone::zone()
		: name{ "NULL" }, bounds{}, entities{}, objects{} {}

	zone::zone(std::string name, collision::Vec bounds, std::vector<object> objects)
		: name{ name }, bounds{ bounds }, entities{}, objects{ objects } {}

	std::pair<zone::id, zone> zone::load(json data) {
		auto id = data["id"].get<uint32_t>();
		auto name = data["name"].get<std::string>();
		auto bounds = collision::Vec{
			data["bounds"]["x"].get<double>(),
			data["bounds"]["y"].get<double>()
		};
		auto objects = std::vector<zone::object>{ data["objects"].size() };
		for (const auto& [k, v] : data["objects"].items()) {
			objects.push_back(static_cast<zone::object>(static_cast<collision::AABB>(v["box"])));
		}
		
		return std::make_pair(id, zone{ name, bounds, objects });
	}

	std::unordered_set<entt::entity>& zone::get_entities() { return entities; }
	std::unordered_set<entt::entity> const& zone::get_entities() const { return entities; }
	std::vector<zone::object>& zone::get_objects() { return objects; }
	std::vector<zone::object> const& zone::get_objects() const { return objects; }
	collision::Vec zone::get_bounds() const { return bounds; }

	map::map()
		: zone_ids{} , zones{} {}

	map map::load(json data)
	{
		try {
			map map;
			map.zones.reserve(data["zones"].size());
			for (auto& [k, v] : data["zones"].items()) {
				map.zones.insert(zone::load(v));
			}
			INFO("MAP", "Loaded {} zones", map.zones.size());
			return map;
		}
		catch (json::exception e) {
			spdlog::error("MAP", "Error while loading: {}", e.what());
			abort();
		}
	}

	void map::insert(entt::entity e, zone::id id) {
		zones[id].get_entities().insert(e);
		zone_ids.insert_or_assign(e, id);
	}

	void map::erase(entt::entity e) {
		if (auto it = zone_ids.find(e); e != zone_ids.end()) {
			auto& [_, id] = *it;
			zones[id].get_entities().erase(e);
			zone_ids.erase(e);
		}
	}

	void map::transfer(entt::entity e, zone::id to) {
		if (auto it = zone_ids.find(e); e != zone_ids.end()) {
			auto& [_, id] = *it;
			zones[id].get_entities().erase(e);
			zones[to].get_entities().insert(e);
			zone_ids.insert_or_assign(e, to);
		}
	}

	zone const& map::get_zone(entt::entity entity) const {
		return get_zone(get_id(entity));
	}

	const zone empty_zone = {};
	zone const& map::get_zone(zone::id id) const {
		if (auto it = zones.find(id); it != zones.end()) {
			auto& [_, zone] = *it;
			return zone;
		}
		return empty_zone;
	}

	bool map::valid(zone::id id) const {
		return id != std::numeric_limits<zone::id>::max() && zones.find(id) != zones.end();
	}

	zone::id map::get_id(entt::entity e) const {
		auto it = zone_ids.find(e);
		return it != zone_ids.end() ? it->second : std::numeric_limits<zone::id>::max();
	}
}