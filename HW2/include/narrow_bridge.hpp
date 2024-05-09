#pragma once

#include "monitor.h"

#include <chrono>
#include <cstdint>
#include <memory>

class Car;
class Queue;

class NarrowBridge : public Monitor
{
   public:
    using i32 = std::int32_t;

    using time = std::chrono::steady_clock;
    using ms = std::chrono::milliseconds;
    using time_point = std::chrono::time_point<time>;

    NarrowBridge() noexcept;
    ~NarrowBridge() noexcept;

    NarrowBridge(const NarrowBridge&) = delete;
    NarrowBridge& operator=(const NarrowBridge&) = delete;

    NarrowBridge(NarrowBridge&&) noexcept;
    NarrowBridge& operator=(NarrowBridge&&) noexcept;

    i32 travel_time;
    i32 maximum_wait_time;
    i32 id;

    Condition wait_zero{ this };
    Condition wait_one{ this };

    std::unique_ptr<Queue> queue;

    i32 curr_from{ 0 };

    time_point prev;

    void pass(const Car&, i32) noexcept;
    bool can_pass(const Car&, i32) noexcept;
    void wait_for_lane(i32) noexcept;
    bool is_timer_elapsed() noexcept;
};
