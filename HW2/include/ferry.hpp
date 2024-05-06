#pragma once

#include "monitor.h"
#include <cstdint>

class Ferry : public Monitor
{
   public:
    using u8 = std::uint8_t;
    using u32 = std::uint32_t;

    u32 travel_time;
    u32 maximum_wait_time;
    u32 capacity;
};
