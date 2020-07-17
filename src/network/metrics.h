#pragma once

#include <atomic>
#include <boost/core/noncopyable.hpp>
#include <chrono>
#include <string>

namespace network {

class metrics : private boost::noncopyable
{
  public:
    struct measurement
    {
        size_t connections;
        size_t read;
        size_t written;
        int time_passed;

        std::string to_string();
    };

  public:
    static void connection();
    static void disconnection();
    static void read(size_t bytes);
    static void written(size_t bytes);
    static measurement measure();

  private:
    metrics();
    static metrics& instance();

    void connection_internal();
    void disconnection_internal();
    void read_internal(size_t bytes);
    void written_internal(size_t bytes);
    measurement measure_internal();

  private:
    std::atomic<size_t> connections_;
    std::atomic<size_t> read_;
    std::atomic<size_t> written_;

    measurement last_measurement_;
    std::chrono::steady_clock::time_point last_measured_at_;
};

} // namespace network
