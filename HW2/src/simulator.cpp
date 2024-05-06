#include "simulator.hpp"

#include "WriteOutput.h"

#include <cassert>
#include <cstddef>
#include <cstdlib>
#include <iostream>
#include <pthread.h>
#include <string>

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

void* Simulator::Car::car_routine(void* arg)
{
    assert(arg != nullptr);
    Car& car{ *static_cast<Car*>(arg) };
    pthread_exit(nullptr);
}

void Simulator::Car::get_path() noexcept
{
    u32 path_length;
    std::cin >> path_length;
    path.resize(path_length);
    for (auto& p : path)
    {
        std::string connector_str;
        std::cin >> connector_str >> p.from >> p.to;
        p.connector_type = to_connector(connector_str);
    }
}

[[nodiscard]] Simulator::Car::connector_ptr constexpr Simulator::Car::
  to_connector(const std::string& str) const noexcept
{
    switch (str[0])
    {
        case 'N':
        {
            return &sim->narrow_bridges[to_uint(str[1])];
        }
        case 'F':
        {
            return &sim->ferries[to_uint(str[1])];
        }
        case 'C':
        {
            return &sim->crossroads[to_uint(str[1])];
        }
        default:
        {
            assert(0 && "unreachable");
        }
    }
}

[[nodiscard]] Simulator::u32 constexpr Simulator::Car::to_uint(
  const char& c) noexcept
{
    return static_cast<u32>(c - '0');
}
