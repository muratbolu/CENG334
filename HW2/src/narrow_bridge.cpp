#include "narrow_bridge.hpp"

#include "monitor.h"
#include <cassert>
#include <chrono>

void NarrowBridge::pass(const Car& car, i32 from) noexcept
{
    /*
    for (;;)
    {
        if (curr_from == from)
        {
            while (car_passing)
            {
                add_to_queue(car, from);
                cv1.wait();
            }
        }
        else if (is_timer_elapsed())
        {
        }
        else if ()
        {
        }
        else
        {
        }
    }
    */
}
/*
void NarrowBridge::add_to_queue(const Car& car, i32 from) noexcept
{
    __synchronized__;
    if (from == 0)
    {
        from_zero.emplace(&car);
    }
    else if (from == 1)
    {
        from_one.emplace(&car);
    }
    else
    {
        assert(0 && "unreachable");
    }
}
*/

bool constexpr NarrowBridge::is_timer_elapsed() noexcept
{
    /*
    auto now{ std::chrono::system_clock::now() };
    auto elapsed{ std::duration_cast<std::chrono::milliseconds>(now - prev) };
    */
    return true;
}
