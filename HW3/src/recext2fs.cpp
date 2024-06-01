#include "recext2fs.hpp"

#include <iostream>
#include <string>

recext2fs::recext2fs(int argc, char* argv[]) noexcept
{
    if (argc != 3)
    {
        std::cerr << "Usage: " << argv[0]
                  << " <image_location> <data_identifier>" << std::endl;
        return;
    }
    image_location = std::string{ argv[1] };
    data_identifier = std::string{ argv[2] };
}
