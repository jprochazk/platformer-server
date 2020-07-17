#pragma once

#include "game/event.h"
#include "game/system/system_base.h"
#include <entt/entt.hpp>

namespace game {
class world;
namespace system {

class movement : public system_base
{
    std::shared_ptr<world> world;

  public:
    movement(std::shared_ptr<game::world> world);
    void update() override;
};

} // namespace system
} // namespace game