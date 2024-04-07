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
#include <utility>

std::optional<parsed_input> eshell::get_input() noexcept
{
    std::string str;
    std::getline(std::cin, str);
    parsed_input parsed;
    if (std::cin.eof()) // reached EOF
    {
        str = "quit";
        int check{ parse_line(str.data(), &parsed) };
        assert(check != 0);
        return std::make_optional(parsed);
    }
    if (parse_line(str.data(), &parsed) == 0)
    {
        // invalid input, return null
        return std::nullopt;
    }
    // pretty_print(&parsed);
    return std::make_optional(parsed);
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
            waitpid(spawn(p.inputs[0].data.cmd.args), nullptr, 0);
            break;

        case SEPARATOR_PIPE:
            assert(p.num_inputs > 0);
            execute_pipeline(p);
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

// NOLINTNEXTLINE (*-avoid-c-arrays)
pid_t eshell::spawn(char* const argv[MAX_ARGS]) noexcept
{
    assert(argv != nullptr);
    pid_t child{ fork() };
    if (child == 0) // child
    {
        // NOLINTNEXTLINE (*-pointer-arithmetic)
        execvp(argv[0], argv);
    }
    // parent
    return child;
}

bool eshell::is_quit(char* arg) noexcept
{
    assert(arg != nullptr);
    return std::strcmp(arg, "quit") == 0;
}

eshell::fd eshell::create_pipe() noexcept
{
    // NOLINTNEXTLINE (*-avoid-c-arrays)
    int fd[2];
    // NOLINTNEXTLINE (*-array-to-pointer-decay)
    assert(pipe(fd) == 0);
    return std::make_pair(fd[0], fd[1]);
}

void eshell::execute_pipeline(const parsed_input& p) noexcept
{
    assert(p.num_inputs > 0);
    auto pipe = create_pipe();
    if (fork() == 0)
    {
        close(pipe.first);
        dup2(pipe.second, STDOUT_FILENO);
        close(pipe.second);
        // NOLINTNEXTLINE (*-array-to-pointer-decay)
        auto argv{ p.inputs[0].data.cmd.args };
        // NOLINTNEXTLINE (*-pointer-arithmetic)
        execvp(argv[0], argv);
    }
    else
    {
        if (fork() == 0)
        {
            close(pipe.second);
            dup2(pipe.first, STDIN_FILENO);
            close(pipe.first);
            // NOLINTNEXTLINE (*-array-to-pointer-decay)
            auto argv{ p.inputs[1].data.cmd.args };
            // NOLINTNEXTLINE (*-pointer-arithmetic)
            execvp(argv[0], argv);
        }
        else
        {
            close(pipe.first);
            close(pipe.second);
            wait(nullptr);
            wait(nullptr);
        }
    }

    /*
    std::vector<fd> pipes(p.num_inputs - 1);
    for (int i{ 0 }; i < p.num_inputs; ++i)
    {
        if (i < p.num_inputs - 1)
        {
            pipes[i] = create_pipe();
        }
        children[i] = fork();

        if (children[i] == 0) // child
        {
            if (i > 0)
            {
                int output{ pipes[i - 1].second };
                close(output);

                int input{ pipes[i - 1].first };
                dup2(input, STDIN_FILENO);
                close(input);
            }
            if (i < p.num_inputs - 1)
            {
                int input{ pipes[i].first };
                close(input);

                int output{ pipes[i].second };
                dup2(output, STDOUT_FILENO);
                close(output);
            }
            // NOLINTNEXTLINE (*-array-to-pointer-decay)
            auto argv{ p.inputs[i].data.cmd.args };
            // NOLINTNEXTLINE (*-pointer-arithmetic)
            execvp(argv[0], argv);
        }
        else // parent
        {
            if (i < p.num_inputs - 1)
            {
                close(pipes[i].first);
                close(pipes[i].second);
            }
        }
    }
    for (int i{ 0 }; i < p.num_inputs; ++i)
    {
        waitpid(children[i], nullptr, 0);
    }
    */
}
