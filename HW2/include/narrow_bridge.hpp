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

    NarrowBridge() noexcept = default;

    i32 travel_time;
    i32 maximum_wait_time;

    Condition cv1{ this };
    i32 curr_from{ 0 };
    bool car_passing{ false };
    std::queue<Car*> from_zero;
    std::queue<Car*> from_one;
    // std::chrono::time_point<> prev{};

    void pass(const Car&, i32) noexcept;
    bool constexpr is_timer_elapsed() noexcept;
};
