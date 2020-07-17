
#include "game/world.h"
#include "game/system/system_base.h"

namespace game {

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

world::registry_t&
world::get_registry()
{
    return registry;
}

world::registry_t const&
world::get_registry() const
{
    return registry;
}

std::shared_ptr<database::connection>
world::get_database()
{
    return database;
}

world::dispatcher_t&
world::get_dispatcher()
{
    return dispatcher;
}

world::map_t&
world::get_map()
{
    return map;
}

world::map_t const&
world::get_map() const
{
    return map;
}

} // namespace game