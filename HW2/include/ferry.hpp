#pragma once

#include "monitor.h"
#include <cstdint>

class Car;

class Ferry : public Monitor
{
   public:
    using i32 = std::int32_t;

    Ferry() noexcept = default;

    i32 travel_time;
    i32 maximum_wait_time;
    i32 capacity;

    Condition cv1{ this };

    void pass(const Car&, i32) noexcept;
};
