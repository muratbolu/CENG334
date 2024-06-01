#include "recext2fs.hpp"

#include "ext2fs.hpp"
#include "ext2fs_print.hpp"

#include <cstdint>
#include <cstdio>
#include <fstream>
#include <ios>
#include <iostream>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

using u8 = std::uint8_t;
using u32 = std::uint32_t;
using u64 = std::uint64_t;
using i64 = std::int64_t;
using pos = std::ifstream::pos_type;

recext2fs::recext2fs(int argc, char* argv[])
{
    if (argc < 3)
    {
        std::cerr << "Usage: " << argv[0]
                  << " <image_location> <data_identifier>" << std::endl;
        throw std::invalid_argument("Invalid number of arguments");
    }
    image_location = std::string{ argv[1] };
    data_identifier = parse_identifier(argc, argv);

    image.open(image_location,
               std::ios_base::in | std::ios_base::out | std::ios_base::binary);

    if (!image.is_open())
    {
        std::cerr << "Could not open the image: " << image_location
                  << std::endl;
        throw std::invalid_argument(image_location);
    }
}

std::vector<u8> recext2fs::parse_identifier(int argc, char* argv[]) noexcept
{
    int identifier_length{ argc - 2 };
    std::vector<u8> identifier;
    for (int i{ 0 }; i < identifier_length; ++i)
    {
        unsigned temp{ 0 };
        sscanf(argv[i + 2], "%x", &temp);
        identifier.emplace_back(temp);
    }
    return identifier;
}

void recext2fs::recover_bitmap() noexcept
{
    read_super_block();
    print_super_block(&this->super_block);

    ext2_block_group_descriptor bg{ read_block_group_desc(0) };
    print_group_descriptor(&bg);

    // go to block bitmap
    image.seekg(get_block_position(0, bg.block_bitmap));
    for (u32 i{ 0 }; i < this->super_block.blocks_per_group; ++i)
    {
        // TODO: check if a block is used
        // if used, change the corresponding bit to one
        // else, skip
    }
}

void recext2fs::read_super_block() noexcept
{
    pos curr_pos{ 0 };
    // Skip the boot data
    curr_pos += EXT2_BOOT_BLOCK_SIZE;
    image.seekg(curr_pos);

    // read onto superblock
    image.read(reinterpret_cast<char*>(&this->super_block),
               sizeof(ext2_super_block));

    this->block_size = EXT2_UNLOG(this->super_block.log_block_size);
}

ext2_block_group_descriptor recext2fs::read_block_group_desc(
  u32 bg_num) noexcept
{
    pos curr_pos{ 0 };

    curr_pos += get_block_group_position(bg_num);
    image.seekg(curr_pos);

    ext2_block_group_descriptor block_group{};
    // read onto block group
    image.read(reinterpret_cast<char*>(&block_group),
               sizeof(ext2_block_group_descriptor));

    return block_group;
}

i64 constexpr recext2fs::get_block_group_position(u32 bg_num) const noexcept
{
    u32 offset{ 0 };
    if (bg_num == 0 || bg_num == 1 || bg_num % 3 == 0 || bg_num % 5 == 0 ||
        bg_num % 7 == 0)
    {
        // Super block exists for given block group
        offset = EXT2_SUPER_BLOCK_SIZE;
    }
    return get_block_position(bg_num, 0) + offset;
}

i64 constexpr recext2fs::get_block_position(u32 bg_num,
                                            u32 b_num) const noexcept
{
    return static_cast<i64>(EXT2_BOOT_BLOCK_SIZE +
                            bg_num * this->super_block.blocks_per_group *
                              block_size +
                            b_num * block_size);
}
