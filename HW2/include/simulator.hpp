#pragma once

#include <cstdint>
#include <string>
#include <tuple>
#include <variant>
#include <vector>

class Simulator
{
   public:
    Simulator();

   private:
    using u8 = std::uint8_t;
    using u32 = std::uint32_t;

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
        u32 travel_time;
        using connector_ptr = std::variant<NarrowBridge*, Ferry*, Crossroad*>;
        using path_object = std::tuple<connector_ptr, u8, u8>;
        std::vector<path_object> path;
        void get_path(Simulator*) noexcept;
        [[nodiscard]] connector_ptr static to_connector(
          Simulator*,
          const std::string&) noexcept;
    };
    std::vector<Car> cars;

    void parse_input() noexcept;
};
