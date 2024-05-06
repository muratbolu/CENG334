#include "car.hpp"

#include "crossroad.hpp"
#include "ferry.hpp"
#include "narrow_bridge.hpp"
#include "simulator.hpp"

#include "pthread.h"
#include <cassert>
#include <iostream>
#include <string>

void* Car::car_routine(void* arg)
{
    assert(arg != nullptr);
    Car& car{ *static_cast<Car*>(arg) };
    pthread_exit(nullptr);
}

void Car::get_path() noexcept
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

[[nodiscard]] Car::connector_ptr constexpr Car::to_connector(
  const std::string& str) const noexcept
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

[[nodiscard]] Car::u32 constexpr Car::to_uint(const char& c) noexcept
{
    return static_cast<u32>(c - '0');
}
