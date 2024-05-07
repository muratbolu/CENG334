#include "car.hpp"

#include "WriteOutput.h"
#include "crossroad.hpp"
#include "ferry.hpp"
#include "helper.h"
#include "narrow_bridge.hpp"
#include "simulator.hpp"

#include <cassert>
#include <cstddef>
#include <iostream>
#include <pthread.h>
#include <string>
#include <variant>

void* Car::car_routine(void* arg)
{
    assert(arg != nullptr);
    Car& car{ *static_cast<Car*>(arg) };
    for (std::size_t i{ 0 }; i < car.path.size(); ++i)
    {
        auto p{ car.path[i] };
        auto output = [i, p](auto&& e)
        {
            return WriteOutput(static_cast<int>(i),
                               connector_to_char(p.connector_type),
                               p.connector_id,
                               std::forward<decltype(e)>(e));
        };

        output(TRAVEL);
        sleep_milli(car.travel_time);
        output(ARRIVE);
        auto* ct{ &p.connector_type };
        if (auto** nb = std::get_if<NarrowBridge*>(ct))
        {
            (*nb)->pass(car, p.from);
        }
        else if (auto** f = std::get_if<Ferry*>(ct))
        {
            (*f)->pass(car, p.from);
        }
        else if (auto** cr = std::get_if<Crossroad*>(ct))
        {
            (*cr)->pass(car, p.from);
        }
        else
        {
            assert(0 && "unreachable");
        }
    }
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
        p.connector_id = to_int(connector_str[1]);
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

[[nodiscard]] char constexpr Car::connector_to_char(
  const connector_ptr& var) noexcept
{
    if (std::get_if<NarrowBridge*>(&var) != nullptr)
    {
        return 'N';
    }
    if (std::get_if<Ferry*>(&var) != nullptr)
    {
        return 'F';
    }
    if (std::get_if<Crossroad*>(&var) != nullptr)
    {
        return 'C';
    }
    assert(0 && "unreachable");
}

[[nodiscard]] Car::i32 constexpr Car::to_int(const char& c) noexcept
{
    return static_cast<i32>(c - '0');
}

[[nodiscard]] Car::u32 constexpr Car::to_uint(const char& c) noexcept
{
    return static_cast<u32>(c - '0');
}
