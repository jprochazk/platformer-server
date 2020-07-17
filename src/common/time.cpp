
#include "common/time.h"
#include <cstddef>
#include <iomanip>
#include <sstream>

std::tm
localtime(const std::time_t& time)
{
    std::tm tm_snapshot;
#if (defined(WIN32) || defined(_WIN32) || defined(__WIN32__))
    localtime_s(&tm_snapshot, &time);
#else
    localtime_r(&time, &tm_snapshot); // POSIX
#endif
    return tm_snapshot;
}

namespace util {

time::time() {}
time::~time() {}

time::time_point
time::now()
{
    return std::chrono::time_point<time::clock, time::duration>(
      std::chrono::duration_cast<time::duration>(
        time::clock::now().time_since_epoch()));
}

std::time_t
time::to_time_t(const time_point& t)
{
    return std::chrono::system_clock::to_time_t(
      std::chrono::time_point<std::chrono::system_clock,
                              std::chrono::system_clock::duration>(
        std::chrono::duration_cast<std::chrono::system_clock::duration>(
          t.time_since_epoch())));
}

std::string
time::to_string(const time::time_point& t)
{
    auto tm_snapshot = localtime(to_time_t(t));
#if (defined(WIN32) || defined(_WIN32) || defined(__WIN32__))
    std::ostringstream ss;
    ss << std::put_time(const_cast<std::tm*>(&tm_snapshot), "%F %T %z");
    return ss.str();
#else
    char buffer[32];
    std::strftime(buffer, sizeof(buffer), "%F %T %z", &tm_snapshot);
    return buffer;
#endif
}

} // namespace util