#include "narrow_bridge.hpp"

NarrowBridge::NarrowBridge() noexcept
    : cv1(this)
{
}

void NarrowBridge::pass(const Car&, i32) noexcept
{
    ;
}
