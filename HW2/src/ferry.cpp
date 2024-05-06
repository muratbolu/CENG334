#include "ferry.hpp"

Ferry::Ferry() noexcept
    : cv1(this)
{
}

void Ferry::pass(const Car&, i32) noexcept
{
    ;
}
