#pragma once

#include <entt/entt.hpp>

namespace game {
	namespace event {
		class dispatcher {
			entt::dispatcher dispatcher;
		public:
			template<typename Event, auto Callback>
			void subscribe() {
				dispatcher.sink<Event>().connect<Callback>();
			}

			template<typename Event, auto Callback, typename Instance>
			void subscribe(Instance& instance) {
				dispatcher.sink<Event>().connect<Callback>(instance);
			}

			template<typename Event, typename... Args>
			void publish(Args&&... args) {
				dispatcher.trigger<Event>(std::forward<Args>(args)...);
			}
		};
	}
}