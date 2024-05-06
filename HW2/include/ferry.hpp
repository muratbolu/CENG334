#pragma once

#include "monitor.h"
#include <cstdint>

class Ferry : public Monitor
{
   public:
    using i32 = std::int32_t;

    i32 travel_time;
    i32 maximum_wait_time;
    i32 capacity;
};
