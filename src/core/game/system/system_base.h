#pragma once

namespace game {

	class system_base {
	public:
		virtual ~system_base() = default;
		virtual void update() = 0;
	};

} // namespace game
