#pragma once

#include "monitor.h"

#include <chrono>
#include <cstdint>
#include <queue>
#include <utility>

class Car;

class NarrowBridge : public Monitor
{
    using i32 = std::int32_t;

    using time = std::chrono::steady_clock;
    using ms = std::chrono::milliseconds;
    using time_point = std::chrono::time_point<time>;

    using car_queue = std::queue<const Car*>;

   public:
    i32 travel_time;
    i32 maximum_wait_time;
    i32 id;

    NarrowBridge() noexcept;
    ~NarrowBridge() noexcept;

    NarrowBridge(const NarrowBridge&) = delete;
    NarrowBridge& operator=(const NarrowBridge&) = delete;

    NarrowBridge(NarrowBridge&&) noexcept;
    NarrowBridge& operator=(NarrowBridge&&) noexcept;

    void pass(const Car&, i32) noexcept;

   private:
    Condition wait_zero{ this };
    Condition wait_one{ this };

    car_queue from_zero;
    car_queue from_one;

    struct
    {
        i32 curr_from{ 0 };
        std::queue<std::pair<const Car*, i32>> curr_passing;
    } lane;
};
