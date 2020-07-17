
#include "game/system/movement.h"
#include "game/event.h"
#include "game/component.h"
#include "game/world.h"
#include "game/collision.h"
#include "common/log.h"

namespace game {
namespace system {

movement::movement(std::shared_ptr<game::world> world) 
	: world(world)
{
	INFO("MOVEMENT", "Initialized");
}

void movement::update() {
	//INFO("MOVEMENT", "Update");

	auto& map = world->get_map();
	auto& registry = world->get_registry();

	using namespace component;
	registry.view<input, body>().each(
	[&](const entt::entity& e, const input& input, body& body) {
		if (body.mtype == body::mtype::WALK) {
			// jump state machine
			if (body.jump == body::jump_state::NONE) {
				body.vel.x = 0.0;
				if (input.left) body.vel.x += -body.speed;
				if (input.right) body.vel.x += body.speed;

				if (input.up) {
					body.vel.y = -body.jump_velocity;
					body.jump = body::jump_state::MID;
				}
			}
			else if (body.jump == body::jump_state::MID) {
				if (!input.up) {
					body.jump = body::jump_state::SINGLE;
				}
			}
			else if (body.jump == body::jump_state::SINGLE) {
				if (input.up) {
					body.vel.x = 0.0;
					if (input.left) body.vel.x += -body.speed;
					if (input.right) body.vel.x += body.speed;

					body.vel.y = -body.jump_velocity;
					body.jump = body::jump_state::DOUBLE;
				}
			}
		}
		else if (body.mtype == body::mtype::FLY) {
			body.vel.x = 0.0; body.vel.y = 0.0;
			if (input.left)		body.vel.x -= body.speed;
			if (input.right)	body.vel.x += body.speed;
			if (input.up)		body.vel.y -= body.speed;
			if (input.down)		body.vel.y += body.speed;
		}
	});


	registry.view<body>().each(
	[&](const entt::entity& e, body& body) {
		if (body.mtype == body::mtype::WALK) {
			body.vel.y += body.gravity;
			body.vel.y = glm::min(body.vel.y, body.gravity * 15);
		}
		body.lastPos = body.pos;
		body.pos += body.vel;
	});

	registry.view<body, collider>().each(
	[&](const entt::entity& e, body& body, collider& col) {
		using namespace collision;

		col.box.center = body.pos;
		auto& zone = map.get_zone(e);

		// check against zone bounds
		auto bounds = zone.get_bounds();
		auto min = col.box.center - col.box.half;
		auto max = col.box.center + col.box.half;
		if (min.x < 0) col.box.center.x = col.box.half.x;
		if (min.y < 0) col.box.center.y = col.box.half.y;
		if (max.x > bounds.x) col.box.center.x = bounds.x - col.box.half.x;
		if (max.y > bounds.y) {
			col.box.center.y = bounds.y - col.box.half.y; 
			body.jump = body::jump_state::NONE;
		}

		// check against zone objects
		for (auto& object : zone.get_objects()) {
			auto result = hit(col.box, object);

			if (result.normal.y == -1) {
				body.jump = body::jump_state::NONE;
			}

			col.box.center -= result.pen;
		}

		body.pos = col.box.center;
	});
}

} // namespace system
} // namespace game 