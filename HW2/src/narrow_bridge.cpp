#include "narrow_bridge.hpp"

#include "monitor.h"

NarrowBridge::NarrowBridge() noexcept
    : cv1{ this }
{
}

void NarrowBridge::pass(const Car& car, i32 from) noexcept
{
    __synchronized__
}
