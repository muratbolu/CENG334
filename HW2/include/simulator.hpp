#pragma once

#include <cstdint>
#include <string>
#include <tuple>
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
        using path_object = std::tuple<connector_ptr, u8, u8>;

        Simulator* sim;
        u32 travel_time;
        std::vector<path_object> path;

        void get_path() noexcept;
        [[nodiscard]] connector_ptr constexpr to_connector(
          const std::string&) const noexcept;
        [[nodiscard]] u32 static constexpr to_uint(const char&) noexcept;
    };
    std::vector<Car> cars;
};
