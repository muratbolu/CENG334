#pragma once

#include <cstdint>
#include <pthread.h>
#include <string>
#include <variant>
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

    void parse_input() noexcept;
    void create_car_threads() noexcept;
    void join_car_threads() const noexcept;

    struct NarrowBridge
    {
        u32 travel_time;
        u32 maximum_wait_time;
    };
    std::vector<NarrowBridge> narrow_bridges;

    struct Ferry
    {
        u32 travel_time;
        u32 maximum_wait_time;
        u32 capacity;
    };
    std::vector<Ferry> ferries;

    struct Crossroad
    {
        u32 travel_time;
        u32 maximum_wait_time;
    };
    std::vector<Crossroad> crossroads;

    struct Car
    {
        using connector_ptr = std::variant<NarrowBridge*, Ferry*, Crossroad*>;

        Simulator* sim;
        u32 travel_time;

        struct Destination
        {
            connector_ptr connector_type;
            u32 from;
            u32 to;
        };
        std::vector<Destination> path;

        // Do not mark noexcept!
        static void* car_routine(void*);

        void get_path() noexcept;
        [[nodiscard]] connector_ptr constexpr to_connector(
          const std::string&) const noexcept;
        [[nodiscard]] u32 static constexpr to_uint(const char&) noexcept;
    };
    std::vector<Car> cars;
    std::vector<pthread_t> car_threads;
};
