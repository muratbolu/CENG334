#include "ferry.hpp"

#include "monitor.h"

Ferry::Ferry() noexcept = default;
Ferry::~Ferry() noexcept = default;

Ferry::Ferry(Ferry&&) noexcept = default;
Ferry& Ferry::operator=(Ferry&&) noexcept = default;

void Ferry::pass(const Car& car, i32 from) noexcept
{
    __synchronized__;
}
