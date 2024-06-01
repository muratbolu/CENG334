#include "recext2fs.hpp"

#include "ext2fs.h"
#include "ext2fs_print.hpp"

#include <cmath>
#include <cstdint>
#include <cstdio>
#include <fstream>
#include <ios>
#include <iostream>
#include <string>
#include <vector>

using u8 = recext2fs::u8;

recext2fs::recext2fs(int argc, char* argv[]) noexcept
{
    if (argc < 3)
    {
        std::cerr << "Usage: " << argv[0]
                  << " <image_location> <data_identifier>" << std::endl;
        return;
    }
    image_location = std::string{ argv[1] };
    data_identifier = parse_identifier(argc, argv);

    image.open(image_location,
               std::ios_base::in | std::ios_base::out | std::ios_base::binary);

    if (!image.is_open())
    {
        std::cerr << "Could not open the image: " << image_location
                  << std::endl;
        return;
    }

    std::ifstream::pos_type curr_pos{ 0 };
    // Skip the boot data
    curr_pos += EXT2_BOOT_BLOCK_SIZE;
    image.seekg(curr_pos);

    ext2_super_block super{};
    // read onto superblock
    image.read(reinterpret_cast<char*>(&super), sizeof(ext2_super_block));
    print_super_block(&super);

    u64 block_size{ EXT2_UNLOG(super.log_block_size) };
    auto block_position = [block_size](u32 pos) -> i64
    { return static_cast<i64>(pos * block_size); };

    // Skip the super block
    curr_pos += EXT2_SUPER_BLOCK_SIZE;
    image.seekg(curr_pos);

    ext2_block_group_descriptor block_group{};
    // read onto block group
    image.read(reinterpret_cast<char*>(&block_group),
               sizeof(ext2_block_group_descriptor));
    print_group_descriptor(&block_group);

    // go to block bitmap
    image.seekg(block_position(block_group.block_bitmap));
    for (u32 i{ 0 }; i < super.blocks_per_group; ++i)
    {
        // TODO: check if a block is used
        // if used, change the corresponding bit to one
        // else, skip
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
