#include "eshell.hpp"
#include "parser.h"

#include <cassert>
#include <cstring>
#include <iostream>
#include <optional>
#include <string>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

std::optional<parsed_input> eshell::get_input() noexcept
{
    std::string str;
    std::getline(std::cin, str, '\n');
    parsed_input parsed;
    if (parse_line(str.data(), &parsed) == 0)
    {
        // invalid input, return null
        return std::nullopt;
    }
    // pretty_print(&parsed);
    return std::make_optional(parsed);
}

bool eshell::is_quit(char* arg) noexcept
{
    assert(arg != nullptr);
    return std::strcmp(arg, "quit") == 0;
}

// NOLINTNEXTLINE (*-avoid-c-arrays)
void eshell::execute(char* const argv[MAX_ARGS]) noexcept
{
    assert(argv != nullptr);
    pid_t f{ fork() };
    if (f == 0) // child
    {
        // NOLINTNEXTLINE (*-pointer-arithmetic)
        execvp(argv[0], argv);
    }
    else // parent
    {
        waitpid(f, nullptr, 0);
    }
}

// Returns false if exiting
bool eshell::process_input(std::optional<parsed_input> p_opt) noexcept
{
    if (!p_opt.has_value())
    {
        // invalid input, continue
        return true;
    }

    parsed_input& p{ *p_opt };
    bool return_val{ true };
    SEPARATOR sep{ p.separator };

    switch (sep)
    {
        case SEPARATOR_NONE:
            if (p.num_inputs <= 0) // No input
            {
                break;
            }

            assert(p.num_inputs > 0);
            if (is_quit(p.inputs[0].data.cmd.args[0]))
            {
                return_val = false;
                break;
            }

            assert(p.num_inputs > 0);
            // NOLINTNEXTLINE (*-array-to-pointer-decay)
            execute(p.inputs[0].data.cmd.args);
            break;

        case SEPARATOR_PIPE:
            assert(p.num_inputs > 0);
            break;
        case SEPARATOR_SEQ:
            assert(p.num_inputs > 0);
            break;
        case SEPARATOR_PARA:
            assert(p.num_inputs > 0);
            break;
        default: // default case unreachable
            assert(false);
    }
    free_parsed_input(&p);
    return return_val;
}

/*
if (p.num_inputs == 3 && p.inputs[0].type == INPUT_TYPE_COMMAND &&
    p.inputs[1].type == INPUT_TYPE_PIPELINE &&
    p.inputs[2].type == INPUT_TYPE_COMMAND &&
    std::strcmp(*(p.inputs[1].data.cmd.args), "|") == 0)
{
    ;
}
*/
