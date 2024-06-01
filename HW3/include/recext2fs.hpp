#pragma once

#include <cstdint>
#include <fstream>
#include <string>
#include <vector>

class recext2fs
{
   public:
    using u8 = std::uint8_t;

    explicit recext2fs(int argc, char* argv[]) noexcept;

   private:
    std::string image_location;
    std::ifstream image;
    std::vector<u8> data_identifier;

    std::vector<u8> static parse_identifier(int argc, char* argv[]) noexcept;
};
