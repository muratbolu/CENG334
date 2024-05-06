#pragma once

#include "car.hpp"
#include "crossroad.hpp"
#include "ferry.hpp"
#include "narrow_bridge.hpp"

#include <cstdint>
#include <pthread.h>
#include <vector>

class Simulator
{
   public:
    Simulator() = default;
    ~Simulator() = default;

    Simulator(const Simulator&) = delete;
    Simulator(Simulator&&) = delete;
    Simulator& operator=(const Simulator&) = delete;
    Simulator&& operator=(Simulator&&) = delete;

    void run() noexcept;

   private:
    using u8 = std::uint8_t;
    using u32 = std::uint32_t;

    friend Car;

    void parse_input() noexcept;
    void create_car_threads() noexcept;
    void join_car_threads() const noexcept;

    std::vector<NarrowBridge> narrow_bridges;
    std::vector<Ferry> ferries;
    std::vector<Crossroad> crossroads;
    std::vector<Car> cars;
    std::vector<pthread_t> car_threads;
};
