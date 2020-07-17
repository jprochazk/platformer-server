#pragma once
# include <spdlog/spdlog.h>
# include <stdexcept>

template<typename... Args>
inline std::runtime_error build_exception(std::string_view format, Args&&... args) {
	return std::runtime_error(fmt::format(format, args...));
}

#ifndef NDEBUG
template<typename... Args>
inline void debug_assert(bool expr, std::string_view format, Args&&... args) {
	if (!(expr)) {
		throw build_exception(format, args...);
	}
}
#else
template<typename... Args>
inline void debug_assert(bool expr, std::string_view format, Args&&... args) {}
#endif