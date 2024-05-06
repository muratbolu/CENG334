#include "simulator.hpp"

#include "WriteOutput.h"

#include <cassert>
#include <iostream>
#include <string>

void Simulator::run() noexcept
{
    parse_input();
    InitWriteOutput();
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

void Simulator::Car::get_path() noexcept
{
    u32 path_length;
    std::cin >> path_length;
    path.resize(path_length);
    for (auto& p : path)
    {
        std::string connector_str;
        std::cin >> connector_str >> std::get<1>(p) >> std::get<2>(p);
        std::get<0>(p) = to_connector(connector_str);
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
