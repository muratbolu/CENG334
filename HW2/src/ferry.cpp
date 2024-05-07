#include "ferry.hpp"

#include "monitor.h"

Ferry::Ferry() noexcept
    : cv1{ this }
{
}

void Ferry::pass(const Car& car, i32 from) noexcept
{
    __synchronized__
}
