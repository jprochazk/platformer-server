#pragma once

#include <atomic>
#include <type_traits>

namespace common {

template<typename T>
class sequence
{
  public:
    static_assert(std::is_fundamental<T>::value, "Sequence number type must be fundamental!");
    using number_type = T;

    sequence(number_type start = static_cast<number_type>(0), number_type increment = static_cast<number_type>(1))
      : current_(start)
      , increment_(increment)
    {}

    number_type get()
    {
        auto next = current_.load(std::memory_order_acquire);
        current_.store(current_.load(std::memory_order_acquire) + increment_, std::memory_order_release);
        return next;
    }

    number_type peek() const { return current_.load(std::memory_order_acquire); }

    std::atomic<number_type> current_;
    number_type increment_;
};

} // namespace common
