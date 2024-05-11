#include "car.hpp"

#include "WriteOutput.h"
#include "crossroad.hpp"
#include "ferry.hpp"
#include "helper.h"
#include "narrow_bridge.hpp"
#include "simulator.hpp"

#include <iostream>
#include <pthread.h>
#include <string>
#include <variant>

void* Car::car_routine(void* arg)
{
    Car& car{ *static_cast<Car*>(arg) };
    for (auto& p : car.path)
    {
        auto output = [car, p](auto&& e)
        {
            return WriteOutput(car.id,
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
        p.connector_id = to_int(connector_str.substr(1));
        p.connector_type = to_connector(connector_str);
    }
}

// Removed constexpr because libstdc++ is not updated in lab computers
[[nodiscard]] Car::connector_ptr Car::to_connector(
  const std::string& str) const noexcept
{
    switch (str[0])
    {
        // default return as NarrowBridge
        default:
        case 'N':
        {
            return &sim->narrow_bridges[to_uint(str.substr(1))];
        }
        case 'F':
        {
            return &sim->ferries[to_uint(str.substr(1))];
        }
        case 'C':
        {
            return &sim->crossroads[to_uint(str.substr(1))];
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
    // default return as NarrowBridge
    return 'N';
}

[[nodiscard]] Car::i32 Car::to_int(const std::string& s) noexcept
{
    return static_cast<i32>(std::stoi(s));
}

[[nodiscard]] Car::u32 Car::to_uint(const std::string& s) noexcept
{
    return static_cast<u32>(std::stoi(s));
}
