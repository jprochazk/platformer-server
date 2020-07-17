#pragma once

#include "common/sequence.h"
#include "game/component.h"
#include "game/system/system_base.h"

namespace game {
class world;
namespace system {

class sync : public system_base
{
    std::shared_ptr<world> world;

  public:
    sync(std::shared_ptr<game::world> world);
    void update() override;
};

} // namespace system
} // namespace game