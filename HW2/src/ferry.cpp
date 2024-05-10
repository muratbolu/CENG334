#include "ferry.hpp"

#include "WriteOutput.h"
#include "car.hpp"
#include "helper.h"
#include "monitor.h"

Ferry::Ferry() noexcept = default;
Ferry::~Ferry() noexcept = default;

Ferry::Ferry(Ferry&&) noexcept = default;
Ferry& Ferry::operator=(Ferry&&) noexcept = default;

void Ferry::pass(const Car& car, i32 from) noexcept
{
    ;
}

/*
void Ferry::increment_capacity(i32 from) noexcept
{
    __synchronized__;
    static_cast<bool>(from) ? cap_one++ : cap_zero++;
}
*/

/*
bool Ferry::can_pass(const Car& car, i32 from) noexcept
{
    Lock mutex{ this };
    Condition& curr_cond{ static_cast<bool>(from) ? wait_one : wait_zero };
    i32& curr_cap{ static_cast<bool>(from) ? cap_one : cap_zero };

    if ((curr_cap == capacity))
    {
        WriteOutput(car.id, 'F', this->id, START_PASSING);
        curr_cond.notifyAll();
        mutex.unlock();
        sleep_milli(travel_time);
        mutex.lock();
    }
}
*/

/*
void Ferry::wait_for_departure(i32 from) noexcept
{
    __synchronized__;
    Condition& curr_cond{ static_cast<bool>(from) ? wait_one : wait_zero };

    curr_cond.wait();
}
*/

/*
bool Ferry::is_timer_elapsed() noexcept
{
    // TODO: must return true when it's called for the first time
    time_point now{ time::now() };
    auto elapsed{ std::chrono::duration_cast<ms>(now - prev) };
    if (elapsed > ms{ maximum_wait_time })
    {
        prev = now;
        return true;
    }
    return false;
}
*/
