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
#include <vector>

std::optional<parsed_input> eshell::get_input() noexcept
{
    std::string str;
    std::getline(std::cin, str);
    parsed_input parsed;
    if (std::cin.eof()) // reached EOF, quit immediately
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

            // only one input for no separator case
            assert(p.num_inputs == 1);

            // check for quit
            if (p.inputs[0].type == INPUT_TYPE_COMMAND &&
                is_quit(p.inputs[0].data.cmd.args[0]))
            {
                return_val = false;
                break;
            }
            // either single command or subshell
            execute_single(p);
            break;

        case SEPARATOR_PIPE:
        {
            assert(p.num_inputs > 0);
            auto new_children{ execute_pipeline(p) };
            for (auto&& i : new_children)
            {
                waitpid(i, nullptr, 0);
            }
            break;
        }
        case SEPARATOR_SEQ:
        {
            assert(p.num_inputs > 0);
            execute_sequential(p);
            break;
        }
        case SEPARATOR_PARA:
        {
            assert(p.num_inputs > 0);
            execute_parallel(p);
            break;
        }
        default:
        {
            assert(false && "Separator default case unreachable");
            break;
        }
    }
    free_parsed_input(&p);
    return return_val;
}

pid_t eshell::execute_command(command& c) noexcept
{
    // NOLINTNEXTLINE (*-avoid-c-arrays)
    auto argv{ c.args };
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

void eshell::execute_single(parsed_input& p) noexcept
{
    // only one input for no separator case
    assert(p.num_inputs == 1);
    auto input{ p.inputs[0] };
    switch (input.type)
    {
        case INPUT_TYPE_NON:
        {
            assert(false && "No INPUT_TYPE_NON in execute_single");
            break;
        }
        case INPUT_TYPE_SUBSHELL:
        {
            auto new_children{ execute_subshell(input.data.subshell) };
            for (auto&& i : new_children)
            {
                waitpid(i, nullptr, 0);
            }
            break;
        }
        case INPUT_TYPE_COMMAND:
        {
            waitpid(execute_command(input.data.cmd), nullptr, 0);
            break;
        }
        case INPUT_TYPE_PIPELINE:
        {
            assert(false && "No INPUT_TYPE_PIPELINE in execute_single");
            break;
        }
    }
}

std::vector<pid_t> eshell::execute_subshell(char* sh) noexcept
{
    parsed_input p;
    if (parse_line(sh, &p) == 0)
    {
        assert(false && "Parse error in execute_subshell");
    }
    // pretty_print(&p);
    std::vector<pid_t> children;
    SEPARATOR sep{ p.separator };
    switch (sep)
    {
        case SEPARATOR_NONE:
        {
            execute_single(p);
            break;
        }
        case SEPARATOR_PIPE:
        {
            auto new_children{ execute_pipeline(p) };
            for (auto&& i : new_children)
            {
                children.emplace_back(i);
            }
            break;
        }
        case SEPARATOR_SEQ:
        {
            execute_sequential(p);
            break;
        }
        case SEPARATOR_PARA:
        {
            assert(false && "execute_subshell SEPARATOR_PARA not implemented");
            break;
        }
    }
    return children;
}

std::vector<pid_t> eshell::execute_pipeline(parsed_input& p) noexcept
{
    assert(p.separator == SEPARATOR_PIPE);

    std::vector<pid_t> children;
    std::vector<fd> pipes(p.num_inputs - 1);

    // initialize all pipes
    for (int i{ 0 }; i < p.num_inputs - 1; ++i)
    {
        pipes[i] = create_pipe();
    }

    // spawn children
    for (int i{ 0 }; i < p.num_inputs; ++i)
    {
        children.emplace_back(fork());
        if (children.back() == 0) // child
        {
            // set input, if applicable
            if (i > 0)
            {
                int input{ pipes[i - 1].first };
                dup2(input, STDIN_FILENO);
            }

            // set output, if applicable
            if (i < p.num_inputs - 1)
            {
                int output{ pipes[i].second };
                dup2(output, STDOUT_FILENO);
            }

            // close all other pipes
            for (int j{ 0 }; j < p.num_inputs - 1; ++j)
            {
                int input{ pipes[j].first };
                close(input);
                int output{ pipes[j].second };
                close(output);
            }

            // This is the really complex part
            std::vector<pid_t> new_children;
            switch (p.inputs[i].type)
            {
                case INPUT_TYPE_NON:
                {
                    assert(false &&
                           "No INPUT_TYPE_NON in top_level_pipeline_member");
                    break;
                }
                case INPUT_TYPE_SUBSHELL:
                {
                    assert(false && "TODO");
                    /*
                    parsed_input p;
                    if (parse_line(s.data.subshell, &p) == 0)
                    {
                        assert(false && "Parse error in
                    top_level_pipeline_member");
                    }
                    pretty_print(&p);
                    switch (p.separator)
                    {
                        case SEPARATOR_NONE:
                            execute_single(p);
                            break;
                        case SEPARATOR_PIPE:
                            execute_pipeline(p);
                            break;
                        case SEPARATOR_SEQ:
                            execute_sequential(p);
                            break;
                        case SEPARATOR_PARA:
                            // assert(false && "execute_subshell
                    SEPARATOR_PARA not
                            // implemented");
                            break;
                    }
                    */
                }
                case INPUT_TYPE_COMMAND:
                {
                    // should be no fork here!
                    execvp(p.inputs[i].data.cmd.args[0],
                           p.inputs[i].data.cmd.args);
                    break;
                }
                case INPUT_TYPE_PIPELINE:
                {
                    assert(false && "No INPUT_TYPE_PIPELINE in "
                                    "top_level_pipeline_member");
                    break;
                }
            }
            for (auto&& c : new_children)
            {
                children.emplace_back(c);
            }
        }
    }
    for (int i{ 0 }; i < p.num_inputs - 1; ++i)
    {
        close(pipes[i].first);
        close(pipes[i].second);
    }
    return children;
}

std::vector<pid_t> eshell::execute_pipeline(pipeline& p) noexcept
{
    std::vector<pid_t> children;
    std::vector<fd> pipes(p.num_commands - 1);

    // initialize all pipes
    for (int i{ 0 }; i < p.num_commands - 1; ++i)
    {
        pipes[i] = create_pipe();
    }

    // spawn children
    for (int i{ 0 }; i < p.num_commands; ++i)
    {
        children.emplace_back(fork());
        if (children.back() == 0) // child
        {
            // set input, if applicable
            if (i > 0)
            {
                int input{ pipes[i - 1].first };
                dup2(input, STDIN_FILENO);
            }

            // set output, if applicable
            if (i < p.num_commands - 1)
            {
                int output{ pipes[i].second };
                dup2(output, STDOUT_FILENO);
            }

            // close all other pipes
            for (int j{ 0 }; j < p.num_commands - 1; ++j)
            {
                int input{ pipes[j].first };
                close(input);
                int output{ pipes[j].second };
                close(output);
            }
            execvp(p.commands[i].args[0], p.commands[i].args);
        }
    }
    for (int i{ 0 }; i < p.num_commands - 1; ++i)
    {
        close(pipes[i].first);
        close(pipes[i].second);
    }
    return children;
}

void eshell::execute_sequential(parsed_input& p) noexcept
{
    for (int i{ 0 }; i < p.num_inputs; ++i)
    {
        auto input{ p.inputs[i] };
        switch (input.type)
        {
            case INPUT_TYPE_NON:
            {
                assert(false && "No INPUT_TYPE_NON in execute_sequential");
                break;
            }
            case INPUT_TYPE_SUBSHELL:
            {
                auto new_children{ execute_subshell(input.data.subshell) };
                for (auto&& i : new_children)
                {
                    waitpid(i, nullptr, 0);
                }
                break;
            }
            case INPUT_TYPE_COMMAND:
            {
                waitpid(execute_command(input.data.cmd), nullptr, 0);
                break;
            }
            case INPUT_TYPE_PIPELINE:
            {
                auto new_children{ execute_pipeline(input.data.pline) };
                for (auto&& i : new_children)
                {
                    waitpid(i, nullptr, 0);
                }
                break;
            }
        }
    }
}

void eshell::execute_parallel(parsed_input& p) noexcept
{
    std::vector<pid_t> children;
    for (int i{ 0 }; i < p.num_inputs; ++i)
    {
        auto input{ p.inputs[i] };
        switch (input.type)
        {
            case INPUT_TYPE_NON:
            {
                assert(false && "No INPUT_TYPE_NON in execute_parallel");
                break;
            }
            case INPUT_TYPE_SUBSHELL:
            {
                auto new_children{ execute_subshell(input.data.subshell) };
                for (auto&& i : new_children)
                {
                    children.emplace_back(i);
                }
                break;
            }
            case INPUT_TYPE_COMMAND:
            {
                children.emplace_back(execute_command(input.data.cmd));
                break;
            }
            case INPUT_TYPE_PIPELINE:
            {
                auto new_children{ execute_pipeline(input.data.pline) };
                for (auto&& i : new_children)
                {
                    children.emplace_back(i);
                }
                break;
            }
        }
    }
    for (auto&& i : children)
    {
        waitpid(i, nullptr, 0);
    }
}
