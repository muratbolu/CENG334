#pragma once

#include "monitor.h"

#include <cstdint>
#include <queue>
#include <utility>

class Car;

class Queue : public Monitor
{
   public:
    using i32 = std::int32_t;
    using car_queue = std::queue<const Car*>;

    Queue() noexcept = default;

    void emplace(const Car* car, i32 from) noexcept
    {
        __synchronized__;
        update_curr_queue(from);
        curr_queue.emplace(std::forward<const Car*>(car));
    }
    const Car* front(i32 from) noexcept
    {
        __synchronized__;
        update_curr_queue(from);
        return curr_queue.front();
    }
    void pop(i32 from) noexcept
    {
        __synchronized__;
        update_curr_queue(from);
        curr_queue.pop();
    }
    bool empty(i32 from) noexcept
    {
        __synchronized__;
        update_curr_queue(from);
        return curr_queue.empty();
    }

   private:
    Condition access{ this };

    void update_curr_queue(i32 from) noexcept
    {
        curr_queue = static_cast<bool>(from) ? from_one : from_zero;
    }
    car_queue from_zero;
    car_queue from_one;
    car_queue& curr_queue{ from_zero };
};
