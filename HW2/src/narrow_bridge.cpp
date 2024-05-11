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
    Condition& opp_cond{ static_cast<bool>(from) ? wait_zero : wait_one };
    car_queue& curr_queue{ static_cast<bool>(from) ? from_one : from_zero };
    i32 opp_from{ static_cast<bool>(from) ? 0 : 1 };

    curr_queue.emplace(&car);

    for (;;)
    {
        if (lane.curr_from == from)
        {
            if (curr_queue.front() == &car)
            {
                if (!lane.curr_passing.empty())
                {
                    mutex.unlock();
                    sleep_milli(PASS_DELAY);
                    mutex.lock();
                }

                WriteOutput(car.id, 'N', this->id, START_PASSING);

                curr_queue.pop();
                lane.curr_passing.emplace(&car);
                curr_cond.notifyAll();

                mutex.unlock();
                sleep_milli(travel_time);
                mutex.lock();

                assert(lane.curr_passing.front() == &car);
                lane.curr_passing.pop();
                if (lane.curr_passing.empty())
                {
                    lane.curr_from = opp_from;
                    opp_cond.notifyAll();
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
        else if (lane.curr_passing.empty())
        {
            lane.curr_from = from;
            continue;
        }
        else
        {
            // TODO: timed wait
            curr_cond.wait();
            continue;
        }
        /*
        else if (!lane_busy)
        {
            curr_from = from;
            car_passed_before = false;
            curr_cond.notifyAll();
            continue;
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
                continue;
            }
            else if (rc == ETIMEDOUT)
            {
                curr_from = from;
                car_passed_before = false;
                curr_cond.notifyAll();
                continue;
            }
            else
            {
                assert(0 && "unreachable");
            }
        }
        assert(0 && "unreachable");
        */
    }
}
