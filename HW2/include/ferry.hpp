#pragma once

#include "monitor.h"

#include <chrono>
#include <cstdint>
#include <queue>

class Car;

class Ferry : public Monitor
{
    using i32 = std::int32_t;

    using time = std::chrono::steady_clock;
    using ms = std::chrono::milliseconds;
    using time_point = std::chrono::time_point<time>;

    using car_queue = std::queue<const Car*>;

   public:
    i32 travel_time;
    i32 maximum_wait_time;
    i32 capacity;
    i32 id;

    Ferry() noexcept;
    ~Ferry() noexcept;

    Ferry(const Ferry&) = delete;
    Ferry& operator=(const Ferry&) = delete;

    Ferry(Ferry&&) noexcept;
    Ferry& operator=(Ferry&&) noexcept;

    void pass(const Car&, i32) noexcept;

   private:
    Condition wait_zero{ this };
    Condition wait_one{ this };

    car_queue from_zero;
    car_queue from_one;
};
