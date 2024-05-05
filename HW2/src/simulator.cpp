#include "simulator.hpp"

#include "WriteOutput.h"

#include <cassert>
#include <cstddef>
#include <cstdlib>
#include <iostream>
#include <string>
#include <utility>

Simulator::Simulator()
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
        std::cin >> c.travel_time;
        c.get_path(this);
    }
}

void Simulator::Car::get_path(Simulator* s) noexcept
{
    u32 path_length;
    std::cin >> path_length;
    path.resize(path_length);
    for (auto& p : path)
    {
        std::string connector_str;
        std::cin >> connector_str >> std::get<1>(p) >> std::get<2>(p);
        std::get<0>(p) = to_connector(s, connector_str);
    }
}

[[nodiscard]] Simulator::Car::connector_ptr Simulator::Car::to_connector(
  Simulator* s,
  const std::string& str) noexcept
{
    switch (str[0])
    {
        case 'N':
        {
            return static_cast<NarrowBridge*>(
              &s->narrow_bridges[static_cast<std::size_t>(str[1] - '0')]);
        }
        case 'F':
        {
            return static_cast<Ferry*>(
              &s->ferries[static_cast<std::size_t>(str[1] - '0')]);
        }
        case 'C':
        {
            return static_cast<Crossroad*>(
              &s->crossroads[static_cast<std::size_t>(str[1] - '0')]);
        }
        default:
        {
            assert(0 && "unreachable");
        }
    }
}
