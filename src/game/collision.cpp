#include "collision.h"

namespace game {
namespace collision {

// Implementation specific functionality
namespace detail {
// Machine epsilon (smallest difference between two T)
template<typename T>
constexpr T epsilon = std::numeric_limits<T>::epsilon();

inline double
sign(const double& value)
{
    return value < 0.0 ? -1.0 : 1.0;
}

inline Vec
sign(const Vec& value)
{
    return Vec{ value.x < 0.0 ? -1.0 : 1.0, value.y < 0.0 ? -1.0 : 1.0 };
}

template<typename T>
inline T
clamp(const T& value, const T& min, const T& max)
{
    if (max < value) {
        return max;
    }

    if (value < min) {
        return min;
    }

    return value;
}
} // namespace detail

double
AABB::left() const
{
    return center.x - half.x;
}
double
AABB::right() const
{
    return center.x + half.x;
}
double
AABB::top() const
{
    return center.y - half.y;
}
double
AABB::bottom() const
{
    return center.y + half.y;
}

// Static collision between AABB and AABB
Result
hit(const AABB& a, const AABB& b)
{
    auto delta = Vec{ b.center.x - a.center.x, b.center.y - a.center.y };
    auto overlap = (a.half + b.half) - glm::abs(delta);
    if (overlap.x <= 0.0 || overlap.y <= 0.0) {
        return Result{ Vec{ 0, 0 }, Vec{ 0, 0 } };
    }

    auto sign = detail::sign(delta);
    if (overlap.x <= overlap.y) {
        auto pen = Vec{ overlap.x * sign.x, 0 };
        auto normal = Vec{ -sign.x, 0 };
        return Result{ normal, pen };
    } else {
        auto pen = Vec{ 0, overlap.y * sign.y };
        auto normal = Vec{ 0, -sign.y };
        return Result{ normal, pen };
    }
}

} // namespace collision
} // namespace game

/*
// Single AABB ray cast
std::optional<Result> cast(const Ray& ray, const AABB& object) {
        /// credit https://noonat.github.io/intersect/

        const auto scale = 1. / ray.delta;
        const auto sign = detail::sign(ray.delta);

        const auto nearTimeX = (object.center.x - (sign.x * object.half.x) -
ray.start.x) * scale.x; const auto nearTimeY = (object.center.y - (sign.y *
object.half.y) - ray.start.y) * scale.y; const auto farTimeX = (object.center.x
+ (sign.x * object.half.x) - ray.start.x) * scale.x; const auto farTimeY =
(object.center.y + (sign.y * object.half.y) - ray.start.y) * scale.y;

        if (nearTimeX > farTimeY || nearTimeY > farTimeX) {
                return std::nullopt;
        }

        const auto nearTime = nearTimeX > nearTimeY ? nearTimeX : nearTimeY;
        const auto farTime = farTimeX < farTimeY ? farTimeX : farTimeY;

        if (nearTime >= 1.0 || farTime <= 0.0) {
                return std::nullopt;
        }

        auto toi = detail::clamp(nearTime, 0.0, 1.0);
        auto normal = nearTimeX > nearTimeY ? Vec{ -sign.x, 0 } : Vec{ 0,
-sign.y }; auto contact = Vec{ ray.start.x + (ray.delta.x * toi), ray.start.y +
(ray.delta.y * toi)
        };
        auto velocity = Vec{
                normal.x == 0.0 ? ray.delta.x : 0.0,
                normal.y == 0.0 ? ray.delta.y : 0.0
        };
        return Result{ normal, contact, velocity, toi };
}
*/

/*
// Multiple AABB ray cast
std::optional<Hit> cast(const Ray& ray, std::vector<AABB>& objects)
{
        std::optional<Hit> nearest = std::nullopt;
        for (auto it = objects.begin(); it != objects.end(); it++) {
                if (auto hit = cast(ray, *it); hit) {
                        // we want the lowest time of impact
                        // which corresponds to the object nearest to ray.start
                        if (!nearest || hit->toi < nearest->toi)
                                nearest = *hit;
                }
        }
        return nearest;
}

// Static collision between Point and AABB
bool hit(const Point& a, const AABB& b) {
        auto min = b.center - b.half;
        auto max = b.center + b.half;
        return a.x >= min.x && a.x <= max.x && a.y >= min.y && a.y <= max.y;
}

*/

/*

std::optional<Hit> sweep(const Swept<AABB>& swept, const std::vector<AABB>&
objects) {
        // inflate each object
        std::vector<AABB> inflated{ objects.begin(), objects.end() };
        for (auto& object : inflated) {
                object.half += swept.object.half;
        }
        // cast the ray through them
        return cast(Ray{ swept.object.center, swept.delta }, inflated);
}
*/
