#pragma once

#include <string>

class recext2fs
{
   public:
    explicit recext2fs(int argc, char* argv[]) noexcept;

   private:
    std::string image_location;
    std::string data_identifier;
};
