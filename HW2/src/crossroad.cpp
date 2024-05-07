#include "crossroad.hpp"

#include "monitor.h"

Crossroad::Crossroad() noexcept
    : cv1{ this }
{
}

void Crossroad::pass(const Car& car, i32 from) noexcept
{
    __synchronized__
}
