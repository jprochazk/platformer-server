#pragma once

#include <spdlog/spdlog.h>

namespace common {
namespace log {
template<typename... Args>
inline void
info(const std::string& origin, const std::string& format, Args&&... args)
{
    return spdlog::info(
      "[{}] => {}", origin, fmt::format(format, std::forward<Args>(args)...));
}

template<typename... Args>
inline void
error(const std::string& origin, const std::string& format, Args&&... args)
{
    return spdlog::error(
      "[{}] => {}", origin, fmt::format(format, std::forward<Args>(args)...));
}

template<typename... Args>
inline void
warn(const std::string& origin, const std::string& format, Args&&... args)
{
    return spdlog::warn(
      "[{}] => {}", origin, fmt::format(format, std::forward<Args>(args)...));
}

template<typename... Args>
inline void
debug(const std::string& origin, const std::string& format, Args&&... args)
{
    return spdlog::debug(
      "[{}] => {}", origin, fmt::format(format, std::forward<Args>(args)...));
}

template<typename... Args>
inline void
trace(const std::string& origin, const std::string& format, Args&&... args)
{
    return spdlog::trace(
      "[{}] => {}", origin, fmt::format(format, std::forward<Args>(args)...));
}
} // namespace log
} // namespace common

#define INFO(origin, format) common::log::info(origin, format);
#define ERR(origin, format) common::log::error(origin, format);
#define WARN(origin, format) common::log::warn(origin, format);
#define DEBUG(origin, format) common::log::debug(origin, format);
#define TRACE(origin, format) common::log::trace(origin, format);

#define INFOF(origin, format, ...)                                             \
    common::log::info(origin, format, __VA_ARGS__);
#define ERRF(origin, format, ...)                                              \
    common::log::error(origin, format, __VA_ARGS__);
#define WARNF(origin, format, ...)                                             \
    common::log::warn(origin, format, __VA_ARGS__);
#define DEBUGF(origin, format, ...)                                            \
    common::log::debug(origin, format, __VA_ARGS__);
#define TRACEF(origin, format, ...)                                            \
    common::log::trace(origin, format, __VA_ARGS__);
