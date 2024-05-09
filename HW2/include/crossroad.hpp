#pragma once

#include "monitor.h"
#include <cstdint>

class Car;

class Crossroad : public Monitor
{
   public:
    using i32 = std::int32_t;

    Crossroad() noexcept = default;

    i32 travel_time;
    i32 maximum_wait_time;
    i32 id;

    Condition cv1{ this };

    void pass(const Car&, i32) noexcept;
};
