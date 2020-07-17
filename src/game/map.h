#pragma once

#include "common/json.h"
#include "game/collision.h"
#include <entt/entt.hpp>
#include <unordered_set>

namespace game {

class zone
{
  public:
    using id = uint32_t;
    using object = collision::AABB;

    zone();
    zone(std::string name, collision::Vec bounds, std::vector<object> objects);
    static std::pair<id, zone> load(json data);

    std::unordered_set<entt::entity>& get_entities();
    std::unordered_set<entt::entity> const& get_entities() const;
    std::vector<object>& get_objects();
    std::vector<object> const& get_objects() const;
    collision::Vec get_bounds() const;

  private:
    std::string name;
    collision::Vec bounds;
    std::unordered_set<entt::entity> entities;
    std::vector<object> objects;
};

class map
{
  public:
    map();

    static map load(json data);

    void insert(entt::entity e, zone::id id = 0);
    void erase(entt::entity e);
    void transfer(entt::entity e, zone::id to);

    zone const& get_zone(entt::entity entity) const;
    zone const& get_zone(zone::id id) const;

    bool valid(zone::id id) const;
    zone::id get_id(entt::entity e) const;

  private:
    std::unordered_map<entt::entity, zone::id> zone_ids;
    std::unordered_map<zone::id, zone> zones = {};
};
} // namespace game