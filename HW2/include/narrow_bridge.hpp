#pragma once

#include "monitor.h"

#include <chrono>
#include <cstdint>
#include <queue>

class Car;

class NarrowBridge : public Monitor
{
   public:
    using i32 = std::int32_t;

    using time = std::chrono::steady_clock;
    using ms = std::chrono::milliseconds;
    using time_point = std::chrono::time_point<time>;

    using car_queue = std::queue<const Car*>;

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

    car_queue from_zero;
    car_queue from_one;

    i32 curr_from{ 0 };

    void pass(const Car&, i32) noexcept;

    void add_car_to_queue(const Car&, i32) noexcept;
    bool can_pass(const Car&, i32) noexcept;
    void wait_for_lane(const Car&, i32) noexcept;

    time_point prev;
    bool is_timer_elapsed() noexcept;
};
