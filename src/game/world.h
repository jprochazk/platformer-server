#pragma once

#include "common/assert.h"
#include "common/ordered_map.h"
#include "database/connection.h"
#include "game/dispatcher.h"
#include "game/map.h"
#include <entt/entt.hpp>
#include <map>
#include <memory>
#include <optional>
#include <string>
#include <utility>

#ifndef NDEBUG
#include <boost/type_index.hpp>
#endif

namespace game {

class system_base;

class world;

class registry : public entt::basic_registry<entt::entity>
{
    friend class world;
    using super = entt::basic_registry<entt::entity>;

  public:
    void destroy(const entity_type entity);
    void destroy(const entity_type entity, const version_type);

    template<typename It>
    void destroy(It first, It last)
    {
        std::copy(first, last, std::back_inserter(destroy_queue_));
    }

  private:
    std::vector<entity_type> destroy_queue_;

    void destroy_queued();
};

class world
{
  public:
    world();
    world(std::optional<database::settings> settings = {}, std::optional<json> map_data = {});

    void add_system(const std::string& name, std::shared_ptr<system_base> system);

    template<typename System>
    std::shared_ptr<System> get_system(const std::string& name);

    void remove_system(const std::string& name);
    void update();
    game::registry& get_registry();
    game::registry const& get_registry() const;
    std::shared_ptr<database::connection> get_database();
    event::dispatcher& get_dispatcher();
    game::map& get_map();
    game::map const& get_map() const;

  private:
    game::registry registry;
    std::shared_ptr<database::connection> database;
    event::dispatcher dispatcher;
    game::map map;
    tsl::ordered_map<std::string, std::shared_ptr<system_base>> systems;
};

template<typename System>
std::shared_ptr<System>
world::get_system(const std::string& name)
{
    auto it = systems.find(name);
    debug_assert(it != systems.end(), "System \"{}\" does not exist", name);
    auto& [_, system] = *it;

#ifndef NDEBUG
    auto ptr = std::dynamic_pointer_cast<System>(system);
    debug_assert(static_cast<bool>(ptr),
                 "System \"{}\" is not of type \"{}\"",
                 name,
                 boost::typeindex::type_id<System>().pretty_name());
#else
    auto ptr = std::static_pointer_cast<System>(system);
#endif
    return ptr;
}

} // namespace game