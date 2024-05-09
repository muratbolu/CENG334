#include "narrow_bridge.hpp"

#include "WriteOutput.h"
#include "car.hpp"
#include "helper.h"
#include "monitor.h"
#include "queue.hpp"

#include <chrono>
#include <memory>

NarrowBridge::NarrowBridge() noexcept
    : queue{ std::make_unique<Queue>() }
{
    ;
}

NarrowBridge::~NarrowBridge() noexcept = default;

NarrowBridge::NarrowBridge(NarrowBridge&&) noexcept = default;
NarrowBridge& NarrowBridge::operator=(NarrowBridge&&) noexcept = default;

void NarrowBridge::pass(const Car& car, i32 from) noexcept
{
    queue->emplace(&car, from);
    for (;;)
    {
        if (can_pass(car, from))
        {
            return;
        }
        wait_for_lane(from);

        /*
        while (curr_from == from)
        {
            while (car_passing)
            {
                wait_for_lane(from);
            }
            if (is_first(car, from))
            {
                ;
            }
        }
        if (is_timer_elapsed() || lane_empty())
        {
            switch_lane(from);
            notify_all(from);
        }
        else
        {
            wait_for_lane(car, from);
        }
        */
    }
}

bool NarrowBridge::can_pass(const Car& car, i32 from) noexcept
{
    __synchronized__; // NOLINT
    Condition& curr_cond{ static_cast<bool>(from) ? wait_one : wait_zero };

    if ((curr_from == from) && (&car == queue->front(from)))
    {
        queue->pop(from);
        sleep_milli(PASS_DELAY);
        WriteOutput(car.id, 'N', this->id, START_PASSING);
        sleep_milli(travel_time);
        WriteOutput(car.id, 'N', this->id, FINISH_PASSING);
        curr_cond.notifyAll();
        return true;
    }
    // queue->empty(!from) means check if opposing queue is empty
    if (is_timer_elapsed() || queue->empty(!from))
    {
        curr_from = from;
        curr_cond.notifyAll();
        return false;
    }

    return false;
}

void NarrowBridge::wait_for_lane(i32 from) noexcept
{
    Lock mutex{ this };
    Condition& curr_cond{ static_cast<bool>(from) ? wait_one : wait_zero };

    // Maybe needed to prevent deadlocks?
    struct timespec timeout
    {
    };
    timeout.tv_nsec = static_cast<__syscall_slong_t>(maximum_wait_time);

    curr_cond.wait();
}

bool NarrowBridge::is_timer_elapsed() noexcept
{
    // TODO: must return true when it's called for the first time
    time_point now{ time::now() };
    auto elapsed{ std::chrono::duration_cast<ms>(now - prev) };
    prev = now;
    return (elapsed > ms{ maximum_wait_time });
}
