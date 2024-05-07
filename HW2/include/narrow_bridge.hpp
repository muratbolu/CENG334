#pragma once

#include "monitor.h"
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
    i32 curr_dir{ 0 };
    std::queue<Car*> zero_to_one;
    std::queue<Car*> one_to_zero;

    void pass(const Car&, i32) noexcept;
};
