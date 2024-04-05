#include "eshell.hpp"
#include "parser.h"

#include <cstring>
#include <iostream>
#include <memory>
#include <sched.h>
#include <string>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <vector>

parsed_input eshell::get_input() noexcept
{
    std::string str;
    std::getline(std::cin, str, '\n');
    parsed_input parsed;
    parse_line(str.data(), &parsed);
    return parsed;
}

// Returns false if exiting
bool eshell::process_input(parsed_input p) noexcept
{
    if (p.num_inputs == 1 && p.inputs[0].type == INPUT_TYPE_COMMAND &&
        std::strcmp(*(p.inputs[0].data.cmd.args), "quit") == 0)
    {
        return false;
    }
    if (p.num_inputs == 1 && p.inputs[0].type == INPUT_TYPE_COMMAND)
    {
        pid_t f{ fork() };
        if (f == 0) // child
        {
            execlp(
              *p.inputs[0].data.cmd.args, *p.inputs[0].data.cmd.args, nullptr);
        }
        else // parent
        {
            int status;
            waitpid(f, &status, 0);
        }
    }
    if (p.num_inputs == 3 && p.inputs[0].type == INPUT_TYPE_COMMAND &&
        p.inputs[1].type == INPUT_TYPE_PIPELINE &&
        p.inputs[2].type == INPUT_TYPE_COMMAND &&
        std::strcmp(*(p.inputs[1].data.cmd.args), "|") == 0)
    {
        ;
    }
    return true;
}
