#pragma once

#include "monitor.h"

#include <array>
#include <cstdint>
#include <queue>
#include <utility>

class Car;

class Crossroad : public Monitor
{
    using i32 = std::int32_t;
    using car_queue = std::queue<const Car*>;

   public:
    i32 travel_time;
    i32 maximum_wait_time;
    i32 id;

    Crossroad() noexcept;
    ~Crossroad() noexcept;

    Crossroad(const Crossroad&) = delete;
    Crossroad& operator=(const Crossroad&) = delete;

    Crossroad(Crossroad&&) noexcept;
    Crossroad& operator=(Crossroad&&) noexcept;

    void pass(const Car&, i32) noexcept;

   private:
    Condition wait_zero{ this };
    Condition wait_one{ this };
    Condition wait_two{ this };
    Condition wait_three{ this };
    std::array<Condition*, 4> waits{ &wait_zero,
                                     &wait_one,
                                     &wait_two,
                                     &wait_three };

    car_queue from_zero;
    car_queue from_one;
    car_queue from_two;
    car_queue from_three;
    std::array<car_queue*, 4> queues{ &from_zero,
                                      &from_one,
                                      &from_two,
                                      &from_three };

    struct
    {
        i32 curr_from{ 0 };
        std::queue<std::pair<const Car*, i32>> curr_passing;
    } lane;
};
