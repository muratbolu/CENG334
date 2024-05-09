#include "crossroad.hpp"

#include "monitor.h"

Crossroad::Crossroad() noexcept = default;
Crossroad::~Crossroad() noexcept = default;

Crossroad::Crossroad(Crossroad&&) noexcept = default;
Crossroad& Crossroad::operator=(Crossroad&&) noexcept = default;

void Crossroad::pass(const Car& car, i32 from) noexcept
{
    __synchronized__;
}
