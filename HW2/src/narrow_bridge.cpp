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

    curr_queue.emplace(&car);
    bool car_passed_before{ false };
    bool lane_busy{ false };

    for (;;)
    {
        if (curr_from == from)
        {
            if (&car == curr_queue.front())
            {
                if (car_passed_before)
                {
                    sleep_milli(PASS_DELAY);
                }
                car_passed_before = true;
                lane_busy = true;
                curr_queue.pop();
                WriteOutput(car.id, 'N', this->id, START_PASSING);
                curr_cond.notifyAll();
                mutex.unlock();
                sleep_milli(travel_time);
                mutex.lock();
                if (curr_queue.empty())
                {
                    lane_busy = false;
                }
                WriteOutput(car.id, 'N', this->id, FINISH_PASSING);
                return;
            }
            else
            {
                curr_cond.wait();
                continue;
            }
        }
        // opposing direction
        else if (curr_queue.size() == 1 && lane_busy)
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

            int rc{ curr_cond.timedwait(&max_wait) };

            if (rc == 0) // notified by another thread, going first!
            {
                continue;
            }
            if (rc == ETIMEDOUT) // timeout, switch
            {
                car_passed_before = false;
                curr_from = from;
                curr_cond.notifyAll();
                continue;
            }
            assert(0 && "unreachable");
        }
        else if (!lane_busy)
        {
            car_passed_before = false;
            curr_from = from;
            curr_cond.notifyAll();
            continue;
        }
        assert(0 && "unreachable");
    }
}
