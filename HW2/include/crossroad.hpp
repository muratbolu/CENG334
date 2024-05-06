#pragma once

#include "monitor.h"
#include <cstdint>

class Car;

class Crossroad : public Monitor
{
   public:
    using i32 = std::int32_t;

    i32 travel_time;
    i32 maximum_wait_time;

    Condition cv1;
    Crossroad() noexcept;
    void pass(const Car&, i32) noexcept;
};
