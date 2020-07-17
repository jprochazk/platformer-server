#pragma once

#include <spdlog/spdlog.h>

namespace common {
	namespace log {
		template<typename... Args>
		inline void info(const std::string& origin, const std::string& format, Args&&... args) {
			return spdlog::info("[{}] => {}", origin, fmt::format(format, std::forward<Args>(args)...));
		}

		template<typename... Args>
		inline void error(const std::string& origin, const std::string& format, Args&&... args) {
			return spdlog::error("[{}] => {}", origin, fmt::format(format, std::forward<Args>(args)...));
		}

		template<typename... Args>
		inline void warn(const std::string& origin, const std::string& format, Args&&... args) {
			return spdlog::warn("[{}] => {}", origin, fmt::format(format, std::forward<Args>(args)...));
		}

		template<typename... Args>
		inline void debug(const std::string& origin, const std::string& format, Args&&... args) {
			return spdlog::debug("[{}] => {}", origin, fmt::format(format, std::forward<Args>(args)...));
		}

		template<typename... Args>
		inline void trace(const std::string& origin, const std::string& format, Args&&... args) {
			return spdlog::trace("[{}] => {}", origin, fmt::format(format, std::forward<Args>(args)...));
		}
	}
}

#define INFO(origin, format, ...) common::log::info(origin, format, __VA_ARGS__);
#define ERR(origin, format, ...) common::log::error(origin, format, __VA_ARGS__);
#define WARN(origin, format, ...) common::log::warn(origin, format, __VA_ARGS__);
#define DEBUG(origin, format, ...) common::log::debug(origin, format, __VA_ARGS__);
#define TRACE(origin, format, ...) common::log::trace(origin, format, __VA_ARGS__);
