#include "recext2fs.hpp"

int main(int argc, char* argv[])
{
    recext2fs fs{ argc, argv };
    fs.recover_bitmap();
    return 0;
}
