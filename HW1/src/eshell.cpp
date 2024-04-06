#include "eshell.hpp"
#include "parser.h"

#include <cstring>
#include <iostream>
#include <string>
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

bool eshell::is_quit(char* const arg) noexcept
{
    return std::strcmp(arg, "quit") == 0;
}

bool eshell::process_one_input(char* const argv[]) noexcept
{
    if (is_quit(argv[0]))
    {
        return false;
    }
    pid_t f{ fork() };
    if (f == 0) // child
    {
        execvp(argv[0], argv);
    }
    else // parent
    {
        waitpid(f, nullptr, 0);
    }
    return true;
}

// Returns false if exiting
bool eshell::process_input(parsed_input p) noexcept
{
    SEPARATOR sep{ p.separator };
    switch (sep)
    {
        case SEPARATOR_NONE:
            if (p.num_inputs <= 0)
            {
                return true;
            }
            else
            {
                return process_one_input(p.inputs[0].data.cmd.args);
            }
        case SEPARATOR_PIPE:
            break;
        case SEPARATOR_SEQ:
            break;
        case SEPARATOR_PARA:
            break;
    } // default case not needed

    if (p.num_inputs == 3 && p.inputs[0].type == INPUT_TYPE_COMMAND &&
        p.inputs[1].type == INPUT_TYPE_PIPELINE &&
        p.inputs[2].type == INPUT_TYPE_COMMAND &&
        std::strcmp(*(p.inputs[1].data.cmd.args), "|") == 0)
    {
        ;
    }
    return true;
}
