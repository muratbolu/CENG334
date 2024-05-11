#include "crossroad.hpp"

#include "WriteOutput.h"
#include "car.hpp"
#include "helper.h"
#include "monitor.h"

#include <cassert>
#include <cerrno>
#include <ctime>

Crossroad::Crossroad() noexcept = default;
Crossroad::~Crossroad() noexcept = default;

Crossroad::Crossroad(Crossroad&&) noexcept = default;
Crossroad& Crossroad::operator=(Crossroad&&) noexcept = default;

void Crossroad::pass(const Car& car, i32 from) noexcept
{
    __synchronized__;

    Condition& curr_cond{ *waits[from % 4] };
    Condition& next_cond{ *waits[(from + 1) % 4] };
    car_queue& curr_queue{ *queues[from % 4] };

    curr_queue.emplace(&car);

    for (;;)
    {
        if (lane.curr_from == from)
        {
            if (curr_queue.front() == &car)
            {
                if (!lane.curr_passing.empty() &&
                    lane.curr_passing.front().second == from)
                {
                    mutex.unlock();
                    sleep_milli(PASS_DELAY);
                    mutex.lock();
                }

                curr_queue.pop();
                lane.curr_passing.emplace(&car, from);

                curr_cond.notifyAll();

                WriteOutput(car.id, 'N', this->id, START_PASSING);

                mutex.unlock();
                sleep_milli(travel_time);
                mutex.lock();

                WriteOutput(car.id, 'N', this->id, FINISH_PASSING);

                assert(lane.curr_passing.front().first == &car);
                lane.curr_passing.pop();
                if (lane.curr_passing.empty())
                {
                    next_cond.notifyAll();
                }
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
            WriteOutput(car.id, 'N', this->id, SWITCHING_LANE);
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
                lane.curr_from = from;
                WriteOutput(car.id, 'N', this->id, SWITCHING_LANE);
                continue;
            }
            else if (rc == ETIMEDOUT)
            {
                // TODO
                lane.curr_from = from;
                WriteOutput(car.id, 'N', this->id, SWITCHING_LANE_TIMEOUT);
                continue;
            }
            else
            {
                assert(0 && "unreachable");
            }
        }
    }
}
