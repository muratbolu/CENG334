#pragma once

#include <cstdint>
#include <string>
#include <variant>
#include <vector>

class Simulator;
class NarrowBridge;
class Ferry;
class Crossroad;

class Car
{
   public:
    using i32 = std::int32_t;
    using u32 = std::uint32_t;

    using connector_ptr = std::variant<NarrowBridge*, Ferry*, Crossroad*>;

    Simulator* sim;
    i32 travel_time;

    struct Destination
    {
        connector_ptr connector_type;
        i32 connector_id;
        i32 from;
        i32 to;
    };
    std::vector<Destination> path;

    // Do not mark noexcept!
    static void* car_routine(void*);

    void get_path() noexcept;
    [[nodiscard]] connector_ptr constexpr to_connector(
      const std::string&) const noexcept;
    [[nodiscard]] char static constexpr connector_to_char(
      const connector_ptr&) noexcept;
    [[nodiscard]] i32 static constexpr to_int(const char&) noexcept;
    [[nodiscard]] u32 static constexpr to_uint(const char&) noexcept;
};
