#include "recext2fs.hpp"

#include <cstdio>
#include <fstream>
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

    std::ifstream image{ image_location };
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
