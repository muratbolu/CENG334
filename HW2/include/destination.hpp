#pragma once

class Destination
{
   public:
    using u8 = std::uint8_t;
    using u32 = std::uint32_t;

    using connector_ptr = std::variant<NarrowBridge*, Ferry*, Crossroad*>;
    connector_ptr connector_type;
    u32 from;
    u32 to;
};