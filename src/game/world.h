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

template<typename Entity>
class registry : public entt::basic_registry<Entity>
{
    friend class world;
    using super = entt::basic_registry<Entity>;
    using entity_t = typename super::entity_type;
    using version_t = typename super::version_type;

    std::vector<entity_t> destroy_queue_;

    void destroy_queued()
    {
        super::destroy(destroy_queue_.begin(), destroy_queue_.end());
        destroy_queue_.clear();
    }

  public:
    void destroy(const entity_t entity) { destroy_queue_.push_back(entity); }

    void destroy(const entity_t entity, const version_t)
    {
        destroy_queue_.push_back(entity);
    }

    template<typename It>
    void destroy(It first, It last)
    {
        std::copy(first, last, std::back_inserter(destroy_queue_));
    }
};

class world
{
  public:
    world();
    world(database::settings settings, json map_data);
    using entity_t = entt::entity;
    using registry_t = registry<entity_t>;
    using dispatcher_t = game::event::dispatcher;
    using map_t = game::map;

    void add_system(const std::string& name,
                    std::shared_ptr<system_base> system);

    template<typename System>
    std::shared_ptr<System> get_system(const std::string& name)
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

    void remove_system(const std::string& name);
    void update();
    registry_t& get_registry();
    registry_t const& get_registry() const;
    std::shared_ptr<database::connection> get_database();
    dispatcher_t& get_dispatcher();
    map_t& get_map();
    map_t const& get_map() const;

  private:
    registry_t registry;
    std::shared_ptr<database::connection> database;
    dispatcher_t dispatcher;
    map_t map;
    tsl::ordered_map<std::string, std::shared_ptr<system_base>> systems;
};

} // namespace game