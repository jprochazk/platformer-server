#pragma once

#include "game/collision.h"
#include "network/socket.h"
#include <cstddef>
#include <cstdint>
#include <entt/entt.hpp>
#include <glm/glm.hpp>
#include <memory>
#include <set>

namespace game {
namespace component {
struct session
{
    uint32_t id;
    std::weak_ptr<network::socket_base> socket;
    std::set<entt::entity> known;
};

struct input
{
    bool up;
    bool down;
    bool left;
    bool right;
};

struct body
{
    using position = glm::vec<2, float>;
    using velocity = glm::vec<2, double>;

    position pos;
    position lastPos;
    velocity vel;
    double gravity;
    double speed;
    double jump_velocity;

    enum class mtype
    {
        WALK,
        FLY
    };
    mtype mtype;

    enum class jump_state
    {
        NONE,
        MID,
        SINGLE,
        DOUBLE
    };
    jump_state jump;
};

struct collider
{
    collision::AABB box;
};
} // namespace component
} // namespace game