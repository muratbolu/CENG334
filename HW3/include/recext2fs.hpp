#pragma once

#include "ext2fs.hpp"

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
    using pos = std::ifstream::pos_type;

    explicit recext2fs(int argc, char* argv[]);
    void recover_bitmap() noexcept;

   private:
    std::string image_location;
    std::ifstream image;
    std::vector<u8> data_identifier;

    ext2_super_block super_block{};
    u64 block_size{};

    std::vector<u8> static parse_identifier(int argc, char* argv[]) noexcept;
    void read_super_block() noexcept;
    ext2_block_group_descriptor read_block_group_desc(u32 bg_num) noexcept;
    i64 constexpr get_block_group_position(u32 bg_num) const noexcept;
    i64 constexpr get_block_position(u32 bg_num, u32 b_num) const noexcept;
};
