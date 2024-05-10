#include "ferry.hpp"

#include "WriteOutput.h"
#include "car.hpp"
#include "helper.h"
#include "monitor.h"

#include <cassert>
#include <cerrno>
#include <ctime>

Ferry::Ferry() noexcept = default;
Ferry::~Ferry() noexcept = default;

Ferry::Ferry(Ferry&&) noexcept = default;
Ferry& Ferry::operator=(Ferry&&) noexcept = default;

void Ferry::pass(const Car& car, i32 from) noexcept
{
    __synchronized__;

    Condition& curr_cond{ static_cast<bool>(from) ? wait_one : wait_zero };
    i32& curr_cap{ static_cast<bool>(from) ? cap_one : cap_zero };

    ++curr_cap;

    if (curr_cap == capacity)
    {
        WriteOutput(car.id, 'F', this->id, START_PASSING);
        curr_cap = 0;
        curr_cond.notifyAll();
        mutex.unlock();
        sleep_milli(travel_time);
        mutex.lock();
        WriteOutput(car.id, 'F', this->id, FINISH_PASSING);
        return;
    }
    else
    {
        struct timespec max_wait;
        clock_gettime(CLOCK_REALTIME, &max_wait);

        max_wait.tv_sec += maximum_wait_time / 1000;
        max_wait.tv_nsec += (maximum_wait_time % 1000) * 1000000;
        if (max_wait.tv_nsec >= 1000000000)
        {
            max_wait.tv_sec += max_wait.tv_nsec / 1000000000;
            max_wait.tv_nsec %= 1000000000;
        }

        int rc{ curr_cond.timedwait(&max_wait) };

        if (rc == 0) // notified
        {
            WriteOutput(car.id, 'F', this->id, START_PASSING);
            mutex.unlock();
            sleep_milli(travel_time);
            mutex.lock();
            WriteOutput(car.id, 'F', this->id, FINISH_PASSING);
            return;
        }
        else if (rc == ETIMEDOUT)
        {
            WriteOutput(car.id, 'F', this->id, START_PASSING);
            curr_cap = 0;
            curr_cond.notifyAll();
            mutex.unlock();
            sleep_milli(travel_time);
            mutex.lock();
            WriteOutput(car.id, 'F', this->id, FINISH_PASSING);
            return;
        }
        else
        {
            assert(0 && "unreachable");
        }
    }
}
