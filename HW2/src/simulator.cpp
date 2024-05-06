#include "simulator.hpp"

#include "WriteOutput.h"
#include "car.hpp"
#include "crossroad.hpp"
#include "ferry.hpp"
#include "narrow_bridge.hpp"

#include <cstddef>
#include <cstdlib>
#include <iostream>
#include <pthread.h>

void Simulator::run() noexcept
{
    InitWriteOutput();
    parse_input();
    create_car_threads();
    join_car_threads();
}

void Simulator::parse_input() noexcept
{
    u32 temp;
    std::cin >> temp;
    narrow_bridges.resize(temp);
    for (auto& n : narrow_bridges)
    {
        std::cin >> n.travel_time >> n.maximum_wait_time;
    }
    std::cin >> temp;
    ferries.resize(temp);
    for (auto& f : ferries)
    {
        std::cin >> f.travel_time >> f.maximum_wait_time >> f.capacity;
    }
    std::cin >> temp;
    crossroads.resize(temp);
    for (auto& c : crossroads)
    {
        std::cin >> c.travel_time >> c.maximum_wait_time;
    }
    std::cin >> temp;
    cars.resize(temp);
    for (auto& c : cars)
    {
        c.sim = this;
        std::cin >> c.travel_time;
        c.get_path();
    }
}

void Simulator::create_car_threads() noexcept
{
    car_threads.resize(cars.size());
    for (std::size_t i{ 0 }; i < cars.size(); ++i)
    {
        pthread_create(&car_threads[i], nullptr, Car::car_routine, &cars[i]);
    }
}

void Simulator::join_car_threads() const noexcept
{
    for (auto&& t : car_threads)
    {
        pthread_join(t, nullptr);
    }
}
