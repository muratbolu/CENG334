#include "crossroad.hpp"

Crossroad::Crossroad() noexcept
    : cv1(this)
{
}

void Crossroad::pass(const Car&, i32) noexcept
{
    ;
}
