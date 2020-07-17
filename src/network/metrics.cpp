
#include "network/metrics.h"
#include "common/log.h"

namespace network {

std::string
metrics::measurement::to_string()
{
    constexpr auto ONE_MB = 1024 * 1024;
    constexpr auto ONE_KB = 1024;
    constexpr auto ONE_B = 1;

    auto runits = (read >= ONE_MB) ? "MB" : (read >= ONE_KB) ? "KB" : "B";
    auto rspace = (read >= ONE_KB) ? "" : " ";
    auto rdivisor =
      (read >= ONE_MB) ? ONE_MB : (read >= ONE_KB) ? ONE_KB : ONE_B;

    auto sunits = (written >= ONE_MB) ? "MB" : (written >= ONE_KB) ? "KB" : "B";
    auto sspace = (written >= ONE_KB) ? "" : " ";
    auto sdivisor =
      (written >= ONE_MB) ? ONE_MB : (written >= ONE_KB) ? ONE_KB : ONE_B;

    auto readActual = ((float)read / rdivisor) * (1000.f / time_passed);
    auto writtenActual = ((float)written / sdivisor) * (1000.f / time_passed);
    return fmt::format("{:.1f} {}/s {}DOWN | {:.1f} {}/s {}UP | {} connections",
                       readActual,
                       runits,
                       rspace,
                       writtenActual,
                       sunits,
                       sspace,
                       connections);
}

metrics::metrics()
  : connections_(0)
  , read_(0)
  , written_(0)
  , last_measurement_()
  , last_measured_at_(std::chrono::steady_clock::now())
{}

metrics&
metrics::instance()
{
    static metrics instance;
    return instance;
}

void
metrics::connection()
{
    return instance().connection_internal();
}
void
metrics::disconnection()
{
    return instance().disconnection_internal();
}
void
metrics::read(size_t bytes)
{
    return instance().read_internal(bytes);
}
void
metrics::written(size_t bytes)
{
    return instance().written_internal(bytes);
}
metrics::measurement
metrics::measure()
{
    return instance().measure_internal();
}

void
metrics::connection_internal()
{
    connections_.store(connections_.load(std::memory_order_acquire) + 1,
                       std::memory_order_release);
}
void
metrics::disconnection_internal()
{
    if (connections_ == 0)
        return;
    connections_.store(connections_.load(std::memory_order_acquire) - 1,
                       std::memory_order_release);
}
void
metrics::read_internal(size_t bytes)
{
    read_.store(read_.load(std::memory_order_acquire) + bytes,
                std::memory_order_release);
}
void
metrics::written_internal(size_t bytes)
{
    written_.store(written_.load(std::memory_order_acquire) + bytes,
                   std::memory_order_release);
}

metrics::measurement
metrics::measure_internal()
{
    auto timePassed =
      static_cast<int>(std::chrono::duration_cast<std::chrono::milliseconds>(
                         std::chrono::steady_clock::now() - last_measured_at_)
                         .count());
    if (timePassed == 0)
        timePassed = 1;

    last_measured_at_ = std::chrono::steady_clock::now();
    last_measurement_ = {
        /*connections = */ connections_.load(std::memory_order_acquire),
        /*read		  = */ read_.load(std::memory_order_acquire),
        /*written	  = */ written_.load(std::memory_order_acquire),
        /*timePassed  = */ timePassed
    };

    read_ = 0;
    written_ = 0;
    return last_measurement_;
}

} // namespace network
