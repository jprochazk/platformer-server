
#pragma once

#include <chrono>
#include <ctime>
#include <string>

namespace util {

class time
{
  private:
    time();

  public:
    virtual ~time() = 0;

    using clock = std::chrono::system_clock;
    using duration = std::chrono::duration<double>;
    using time_point = std::chrono::time_point<clock, duration>;

    static time_point now();
    static std::time_t to_time_t(const time_point& t);
    static std::string to_string(const time_point& t);
}; // class time

} // namespace util
