#include "simulator.hpp"

#include "WriteOutput.h"
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
        std::get<0>(p) = to_connector(std::move(connector_str));
    }
}

Simulator::Car::connector_ptr Simulator::Car::to_connector(
  std::string&& str) noexcept
{
    switch (str[0])
    {
        case 'N': // TODO
        case 'F': // TODO
        case 'C': // TODO
    }
}
