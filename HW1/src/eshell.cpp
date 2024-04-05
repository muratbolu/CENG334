#include "eshell.hpp"
#include "parser.h"

#include <cstring>
#include <iostream>
#include <sched.h>
#include <string>
#include <string_view>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

parsed_input eshell::get_input() noexcept
{
    std::string str;
    std::getline(std::cin, str, '\n');
    parsed_input parsed;
    parse_line(str.data(), &parsed);
    return parsed;
}

bool eshell::process_one_input(std::string_view sv) noexcept
{
    if (sv == "quit")
    {
        return false;
    }
    pid_t f{ fork() };
    if (f == 0) // child
    {
        execlp(sv.data(), sv.data(), nullptr);
    }
    else // parent
    {
        int status;
        waitpid(f, &status, 0);
    }
    return true;
}

// Returns false if exiting
bool eshell::process_input(parsed_input p) noexcept
{
    int num_inputs{ p.num_inputs };
    switch (num_inputs)
    {
        case 0:
            return true;
        case 1:
            return process_one_input(p.inputs[0].data.cmd.args[0]);
        default:
            return true;
    }
    if (p.num_inputs == 3 && p.inputs[0].type == INPUT_TYPE_COMMAND &&
        p.inputs[1].type == INPUT_TYPE_PIPELINE &&
        p.inputs[2].type == INPUT_TYPE_COMMAND &&
        std::strcmp(*(p.inputs[1].data.cmd.args), "|") == 0)
    {
        ;
    }
}
