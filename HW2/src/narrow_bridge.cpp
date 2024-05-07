#include "narrow_bridge.hpp"

#include "monitor.h"

void NarrowBridge::pass(const Car& car, i32 from) noexcept
{
    __synchronized__;
    if (curr_dir == from)
    {
        ;
    }
}
