#include "car.hpp"

#include "WriteOutput.h"
#include "crossroad.hpp"
#include "ferry.hpp"
#include "helper.h"
#include "narrow_bridge.hpp"
#include "simulator.hpp"

#include "pthread.h"
#include <cassert>
#include <cstddef>
#include <iostream>
#include <string>
#include <variant>

void* Car::car_routine(void* arg)
{
    assert(arg != nullptr);
    Car& car{ *static_cast<Car*>(arg) };
    for (std::size_t i{ 0 }; i < car.path.size(); ++i)
    {
        auto output = [i, p = car.path[i]](auto&& e)
        {
            return WriteOutput(static_cast<int>(i),
                               to_connector(p.connector_type),
                               p.connector_id,
                               std::forward<decltype(e)>(e));
        };

        output(TRAVEL);
        sleep_milli(car.travel_time);
        output(ARRIVE);
        // TODO: pass
    }
    pthread_exit(nullptr);
}

void Car::get_path() noexcept
{
    i32 path_length;
    std::cin >> path_length;
    path.resize(path_length);
    for (auto& p : path)
    {
        std::string connector_str;
        std::cin >> connector_str >> p.from >> p.to;
        p.connector_id = to_uint(connector_str[1]);
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

[[nodiscard]] char constexpr Car::to_connector(
  const connector_ptr& var) noexcept
{
    if (std::get_if<NarrowBridge*>(&var))
    {
        return 'N';
    }
    if (std::get_if<Ferry*>(&var))
    {
        return 'F';
    }
    if (std::get_if<Crossroad*>(&var))
    {
        return 'C';
    }
    assert(0 && "unreachable");
}

[[nodiscard]] Car::i32 constexpr Car::to_uint(const char& c) noexcept
{
    return static_cast<i32>(c - '0');
}
