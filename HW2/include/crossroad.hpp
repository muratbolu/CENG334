#pragma once

#include "monitor.h"

#include <chrono>
#include <cstdint>
#include <queue>

class Car;

class Crossroad : public Monitor
{
    using i32 = std::int32_t;

    using time = std::chrono::steady_clock;
    using ms = std::chrono::milliseconds;
    using time_point = std::chrono::time_point<time>;

    using car_queue = std::queue<const Car*>;

   public:
    i32 travel_time;
    i32 maximum_wait_time;
    i32 id;

    Crossroad() noexcept;
    ~Crossroad() noexcept;

    Crossroad(const Crossroad&) = delete;
    Crossroad& operator=(const Crossroad&) = delete;

    Crossroad(Crossroad&&) noexcept;
    Crossroad& operator=(Crossroad&&) noexcept;

    void pass(const Car&, i32) noexcept;

   private:
    Condition wait_zero{ this };
    Condition wait_one{ this };
    Condition wait_two{ this };
    Condition wait_three{ this };

    car_queue from_zero;
    car_queue from_one;
    car_queue from_two;
    car_queue from_three;
};
