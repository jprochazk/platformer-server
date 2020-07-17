#pragma once

#include <cstdint>

namespace network {
	namespace server {
		enum class opcode : uint16_t {
			ID = 0,
			STATE
		};
	}

	namespace client {
		enum class opcode : uint16_t {
			INPUT = 0,
			TOGGLE_MTYPE
		};
	}
}
