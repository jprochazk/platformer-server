#pragma once

#include <entt/entt.hpp>
#include <cstdint>

namespace game {
	namespace event {
		// TODO: batch join/leave
		// may end up shitting on the performance, because it's O(N*M), N = entities, M = systems
		// and there is no data locality, each listener is triggered for every entity

		struct entity_join {
			entt::entity who;
		};

		struct entity_leave {
			entt::entity who;
		};

		// ZONE SYSTEM
		struct zone_transfer {
			entt::entity who;
			uint16_t to;
		};
	}
}