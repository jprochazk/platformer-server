#pragma once

#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/epsilon.hpp>
#include <glm/gtx/perpendicular.hpp>
#include <cstdlib>
#include <cstdint>
#include <cstddef>
#include <vector>
#include <type_traits>
#include <algorithm>
#include <limits>
#include <optional>
#include <functional>

namespace game {
namespace collision {

using Vec = glm::vec<2, double>;

/* SHAPES */

using Point = Vec;

struct Ray {
	Vec start;
	Vec delta;
};

struct AABB {
	Vec center;
	Vec half;

	double left() const;
	double right() const;
	double top() const;
	double bottom() const;
};

/* HELPERS */

/*
template<typename T = void>
using Callback = std::function<void(Result, T*)>;

template<typename Shape, typename T = void>
struct Collider {
	Shape shape;
	T* owner;
};
*/

/* COLLISION */

struct Result {
	Vec normal;
	Vec pen;
};

Result hit(const AABB& a, const AABB& b);

/*

template<typename Shape>
struct Swept {
	Shape shape;
	Vec velocity;
};

struct Result {
	// Normal of the collision
	Vec normal;
	// Contact point
	Vec contact;
	// Remaining velocity (for sliding along shapes)
	Vec velocity;
	// Time of impact (for determining nearest collision)
	double time;
};
*/

//Vec cast(const Ray& ray, const AABB& object);


/*
template<typename T = void>
bool test(Swept<AABB> swept, std::vector<Collider<AABB, T>>& colliders, Callback<T> callback) {
	// defaults
	auto nearestResult = Result{ Vec{},Vec{},Vec{},std::numeric_limits<double>::max() };
	auto nearestObject = colliders.end();
	// cast the ray through every object, finding the nearest hit
	if (swept.velocity.x == 0 && swept.velocity.y == 0) {
		// TODO: maybe consider static collision...
		// for now, just prevent object from moving
		return true;
	}
	else {
		// swept collision
		auto ray = Ray{ swept.shape.center, swept.velocity };
		for (auto it = colliders.begin(); it != colliders.end(); it++) {
			auto result = cast(ray, AABB{ it->shape.center, it->shape.half + swept.shape.half });
			if (result && result->time < nearestResult.time) {
				nearestResult = *result;
				nearestObject = it;
			}
		}
	}

	if (nearestObject == colliders.end()) {
		return false;
	}
	// if we get a hit, call the callback
	callback(nearestResult, nearestObject->owner);
	return true;
}
*/

} // namespace collision
} // namespace game