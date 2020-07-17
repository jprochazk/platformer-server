#pragma once

#include "common/json.h"
#include "game/world.h"
#include <cstdint>
#include <entt/entt.hpp>
#include <functional>

namespace network {
namespace packet {
struct handler
{
    using function_type =
      std::function<void(game::world&, const entt::entity&, const json&)>;

    handler(uint16_t opcode, function_type fn);

    const uint16_t opcode;
    const function_type fn;
    void operator()(game::world& world,
                    const entt::entity& entity,
                    const json& packet);
};

class handler_table
{
  public:
    virtual ~handler_table() = default;
    virtual handler const& get(uint16_t opcode) = 0;
};

class default_handler_table : public handler_table
{
  public:
    handler const& get(uint16_t opcode) override;
};
} // namespace packet
} // namespace network