#include "narrow_bridge.hpp"

#include "WriteOutput.h"
#include "car.hpp"
#include "helper.h"
#include "monitor.h"

#include <cassert>
#include <cerrno>
#include <ctime>

NarrowBridge::NarrowBridge() noexcept = default;
NarrowBridge::~NarrowBridge() noexcept = default;

NarrowBridge::NarrowBridge(NarrowBridge&&) noexcept = default;
NarrowBridge& NarrowBridge::operator=(NarrowBridge&&) noexcept = default;

void NarrowBridge::pass(const Car& car, i32 from) noexcept
{
    __synchronized__;

    Condition& curr_cond{ static_cast<bool>(from) ? wait_one : wait_zero };
    car_queue& curr_queue{ static_cast<bool>(from) ? from_one : from_zero };
    car_queue& opp_queue{ static_cast<bool>(from) ? from_zero : from_one };

    curr_queue.emplace(&car);
    int rc{ ETIMEDOUT };

    for (;;)
    {

        if (curr_from == from)
        {
            if (&car == curr_queue.front())
            {
                curr_queue.pop();
                if (rc == 0)
                {
                    sleep_milli(PASS_DELAY);
                }
                WriteOutput(car.id, 'N', this->id, START_PASSING);
                curr_cond.notifyAll();
                mutex.unlock();
                sleep_milli(travel_time);
                mutex.lock();
                WriteOutput(car.id, 'N', this->id, FINISH_PASSING);
                return;
            }
        }
        else if (opp_queue.empty())
        {
            curr_from = from;
            curr_cond.notifyAll();
            continue;
        }
        else if (curr_queue.size() == 1)
        {
            // Set timer
            struct timespec max_wait;
            clock_gettime(CLOCK_REALTIME, &max_wait);

            max_wait.tv_sec += maximum_wait_time / 1000;
            max_wait.tv_nsec += (maximum_wait_time % 1000) * 1000000;
            if (max_wait.tv_nsec >= 1000000000)
            {
                max_wait.tv_sec += max_wait.tv_nsec / 1000000000;
                max_wait.tv_nsec %= 1000000000;
            }

            rc = curr_cond.timedwait(&max_wait);

            if (rc == 0) // notified by another thread
            {
                continue;
            }
            if (rc == ETIMEDOUT)
            {
                curr_from = from;
                curr_cond.notifyAll();
                continue;
            }
            assert(0 && "unreachable");
        }
        else
        {
            curr_cond.wait();
        }
    }
}

bool NarrowBridge::can_pass(const Car& car, i32 from) noexcept
{
    __synchronized__; // NOLINT
    Condition& curr_cond{ static_cast<bool>(from) ? wait_one : wait_zero };
    car_queue& curr_queue{ static_cast<bool>(from) ? from_one : from_zero };
    car_queue& opp_queue{ static_cast<bool>(from) ? from_zero : from_one };

    if ((curr_from == from) && (&car == curr_queue.front()))
    {
        curr_queue.pop();
        sleep_milli(PASS_DELAY);
        WriteOutput(car.id, 'N', this->id, START_PASSING);
        sleep_milli(travel_time);
        WriteOutput(car.id, 'N', this->id, FINISH_PASSING);
        curr_cond.notifyAll();
        return true;
    }
    // deprecated: queue->empty(!from) means check if opposing queue is empty
    if (opp_queue.empty())
    {
        curr_from = from;
        curr_cond.notifyAll();
        return false;
    }

    return false;
}

void NarrowBridge::wait_for_lane(const Car& car, i32 from) noexcept
{
    __synchronized__; // NOLINT
    Condition& curr_cond{ static_cast<bool>(from) ? wait_one : wait_zero };
    car_queue& curr_queue{ static_cast<bool>(from) ? from_one : from_zero };

    /* Maybe needed to prevent deadlocks?
    struct timespec timeout
    {
    };
    timeout.tv_nsec = static_cast<__syscall_slong_t>(maximum_wait_time);
    */

    // checks if going next, skips waiting
    if ((curr_from == from) && (&car == curr_queue.front()))
    {
        return;
    }
    curr_cond.wait();
}
