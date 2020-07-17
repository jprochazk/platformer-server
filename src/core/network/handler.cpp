
#include "handler.h"
#include "common/log.h"
#include "common/enum_util.h"
#include "common/json.h"
#include "network/opcode.h"
#include "game/component.h"

namespace network {
	namespace packet {
		// void handle_(world& world, const entt::entity& entity, const json& packet)

		void handle_input(game::world& world, const entt::entity& entity, const json& packet) {
			using namespace game;

			auto& registry = world.get_registry();

			if (!packet.contains("f")) return;
			uint8_t flags = packet["f"];

			if (!registry.has<component::input>(entity)) return;
			auto& state = registry.get<component::input>(entity);

			if ((flags & 1) == 1) state.up = true;
			else state.up = false;
			if ((flags & 2) == 2) state.down = true;
			else state.down = false;
			if ((flags & 4) == 4) state.left = true;
			else state.left = false;
			if ((flags & 8) == 8) state.right = true;
			else state.right = false;
		}

		void handle_toggle_mtype(game::world& world, const entt::entity& entity, const json& packet) {
			using namespace game;

			auto& registry = world.get_registry();

			if (!registry.has<component::body>(entity)) return;
			auto& body = registry.get<component::body>(entity);

			if (body.mtype == component::body::mtype::WALK) {
				body.mtype = component::body::mtype::FLY;
			}
			else {
				body.mtype = component::body::mtype::WALK;
			}
		}

		handler::handler(uint16_t opcode, handler::function_type fn)
			: opcode(opcode), fn(fn) {}

		void handler::operator()(game::world& world, const entt::entity& entity, const json& packet) {
			if (!fn) return;
			fn(world, entity, packet);
		}

		static const std::vector<handler> HANDLER_TABLE = {
			{ common::to_underlying(network::client::opcode::INPUT), &handle_input },
			{ common::to_underlying(network::client::opcode::TOGGLE_MTYPE), &handle_toggle_mtype },
		};
		static const handler DEFAULT_HANDLER = {
			(uint16_t)-1, nullptr
		};

		const handler& default_handler_table::get(uint16_t value) {
			auto opcode = common::from_underlying<network::client::opcode>(value);
			if (!opcode.has_value()) return DEFAULT_HANDLER;
			return HANDLER_TABLE[value];
		}
	}
}