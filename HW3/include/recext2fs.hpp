#pragma once

#include <cstdint>
#include <fstream>
#include <string>
#include <vector>

class recext2fs
{
   public:
    using u8 = std::uint8_t;
    using u32 = std::uint32_t;
    using u64 = std::uint64_t;
    using i64 = std::int64_t;

    explicit recext2fs(int argc, char* argv[]);
    void recover_bitmap() noexcept;

   private:
    std::string image_location;
    std::ifstream image;
    std::vector<u8> data_identifier;

    std::vector<u8> static parse_identifier(int argc, char* argv[]) noexcept;
};
