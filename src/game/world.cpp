
#include "game/world.h"
#include "game/system/system_base.h"

namespace game {

void
registry::destroy_queued()
{
    super::destroy(destroy_queue_.begin(), destroy_queue_.end());
    destroy_queue_.clear();
}

void
registry::destroy(const entity_type entity)
{
    destroy_queue_.push_back(entity);
}

void
registry::destroy(const entity_type entity, const version_type)
{
    destroy_queue_.push_back(entity);
}

world::world()
  : registry{}
  , database{}
  , dispatcher{}
  , map{}
  , systems{}
{}

world::world(std::optional<database::settings> settings,
             std::optional<json> map_data)
  : registry{}
  , database{}
  , dispatcher{}
  , map{}
  , systems{}
{
    if (settings)
        database = std::make_shared<database::connection>(*settings);

    if (map_data)
        map = map::load(*map_data);
}

void
world::add_system(const std::string& name, std::shared_ptr<system_base> system)
{
    debug_assert(systems.find(name) == systems.end(),
                 "System \"{}\" already exists",
                 name);

    systems.insert(std::make_pair(name, system));
}

void
world::remove_system(const std::string& name)
{
    systems.erase(name);
}

void
world::update()
{
    for (auto& [_, system] : systems) {
        system->update();
    }

    registry.destroy_queued();
}

game::registry&
world::get_registry()
{
    return registry;
}

game::registry const&
world::get_registry() const
{
    return registry;
}

std::shared_ptr<database::connection>
world::get_database()
{
    return database;
}

event::dispatcher&
world::get_dispatcher()
{
    return dispatcher;
}

game::map&
world::get_map()
{
    return map;
}

game::map const&
world::get_map() const
{
    return map;
}

} // namespace game