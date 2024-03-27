#include "eshell.hpp"
#include "parser.h"

#include <cstring>
#include <iostream>
#include <memory>
#include <string>
#include <vector>

parsed_input eshell::get_input() noexcept
{
    std::string str;
    std::getline(std::cin, str, '\n');
    std::unique_ptr<parsed_input> parsed = std::make_unique<parsed_input>();
    std::vector<char> input(str.cbegin(), str.cend());
    parse_line(input.data(), parsed.get());
    return *parsed;
}

// Returns false if exiting
bool eshell::process_input(parsed_input p) noexcept
{
    if (p.num_inputs == 1 && p.inputs[0].type == INPUT_TYPE_COMMAND &&
        std::strcmp(*(p.inputs[0].data.cmd.args), "quit") == 0)
    {
        return false;
    }
    return true;
}
