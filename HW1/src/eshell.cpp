#include "eshell.hpp"
#include "parser.h"

#include <array>
#include <cassert>
#include <climits>
#include <cstddef>
#include <cstdlib>
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
        {
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
        }
        case SEPARATOR_PIPE:
        {
            assert(p.num_inputs > 0);
            execute_pipeline(p);
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

bool eshell::is_quit(char* arg) noexcept
{
    assert(arg != nullptr);
    return std::strcmp(arg, "quit") == 0;
}

eshell::fd eshell::create_pipe() noexcept
{
    int fd[2];             // NOLINT
    assert(pipe(fd) == 0); // NOLINT
    return std::make_pair(fd[0], fd[1]);
}

void eshell::set_pipes(const std::vector<fd>& pipes, int i) noexcept
{
    // set input, if applicable
    if (i > 0)
    {
        int input{ pipes[i - 1].first };
        dup2(input, STDIN_FILENO);
    }

    // set output, if applicable
    if (i < static_cast<int>(pipes.size()))
    {
        int output{ pipes[i].second };
        dup2(output, STDOUT_FILENO);
    }

    // close all other pipes
    for (auto&& j : pipes)
    {
        int input{ j.first };
        close(input);
        int output{ j.second };
        close(output);
    }
};

void eshell::set_repeater_pipes(const std::vector<fd>& pipes, int i) noexcept
{
    // set input
    int input{ pipes[i].first };
    dup2(input, STDIN_FILENO);

    // output as STDOUT

    // close all other pipes
    for (auto&& j : pipes)
    {
        int input{ j.first };
        close(input);
        int output{ j.second };
        close(output);
    }
}

[[noreturn]] void eshell::repeater_procedure(std::vector<fd> pipes) noexcept
{
    // input as STDIN
    ssize_t n{ 0 };
    std::array<char, INPUT_BUFFER_SIZE> buffer;
    while (((n = read(STDIN_FILENO, buffer.data(), INPUT_BUFFER_SIZE)) != 0) &&
           (!pipes.empty()))
    {
        // output to all pipes
        for (std::size_t i{ 0 }; i < pipes.size();)
        {
            if (write(pipes[i].second, buffer.data(), n) == 0)
            {
                int input{ pipes[i].first };
                close(input);
                int output{ pipes[i].second };
                close(output);
                pipes.erase(pipes.begin() +
                            static_cast<int>(i)); // delete i^th element
                // continue without incrementing i
            }
            else
            {
                ++i;
            }
        }
    }
    for (auto&& j : pipes)
    {
        int input{ j.first };
        close(input);
        int output{ j.second };
        close(output);
    }
    exit(EXIT_SUCCESS);
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
            // NOLINTNEXTLINE
            auto new_children{ fork_subshell(input.data.subshell) };
            for (auto&& i : new_children)
            {
                waitpid(i, nullptr, 0);
            }
            break;
        }
        case INPUT_TYPE_COMMAND:
        {
            waitpid(fork_command(input.data.cmd), nullptr, 0);
            break;
        }
        case INPUT_TYPE_PIPELINE:
        {
            assert(false && "No INPUT_TYPE_PIPELINE in execute_single");
            break;
        }
    }
}

void eshell::execute_pipeline(parsed_input& p) noexcept
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
        switch (p.inputs[i].type) // NOLINT
        {
            case INPUT_TYPE_NON:
            {
                assert(false && "No INPUT_TYPE_NON in execute_pipeline");
                break;
            }
            case INPUT_TYPE_SUBSHELL:
            {
                auto new_children{ fork_and_pipe_subshell(
                  p.inputs[i].data.subshell, pipes, i) }; // NOLINT
                for (auto&& i : new_children)
                {
                    children.emplace_back(i);
                }
                break;
            }
            case INPUT_TYPE_COMMAND:
            {
                children.emplace_back(
                  fork_and_pipe(p.inputs[i].data.cmd, pipes, i)); // NOLINT
                break;
            }
            case INPUT_TYPE_PIPELINE:
            {
                assert(false && "No INPUT_TYPE_PIPELINE in execute_pipeline");
                break;
            }
        }
    }
    for (int i{ 0 }; i < p.num_inputs - 1; ++i)
    {
        close(pipes[i].first);
        close(pipes[i].second);
    }
    for (auto&& i : children)
    {
        waitpid(i, nullptr, 0);
    }
}

void eshell::execute_sequential(parsed_input& p) noexcept
{
    for (int i{ 0 }; i < p.num_inputs; ++i)
    {
        auto input{ p.inputs[i] }; // NOLINT
        switch (input.type)
        {
            case INPUT_TYPE_NON:
            {
                assert(false && "No INPUT_TYPE_NON in execute_sequential");
                break;
            }
            case INPUT_TYPE_SUBSHELL:
            {
                assert(false && "No INPUT_TYPE_SUBSHELL in execute_sequential");
                /*
                auto new_children{ fork_subshell(input.data.subshell) };
                for (auto&& i : new_children)
                {
                    waitpid(i, nullptr, 0);
                }
                */
                break;
            }
            case INPUT_TYPE_COMMAND:
            {
                waitpid(fork_command(input.data.cmd), nullptr, 0);
                break;
            }
            case INPUT_TYPE_PIPELINE:
            {
                auto new_children{ fork_pipeline(input.data.pline) };
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
        auto input{ p.inputs[i] }; // NOLINT
        switch (input.type)
        {
            case INPUT_TYPE_NON:
            {
                assert(false && "No INPUT_TYPE_NON in execute_parallel");
                break;
            }
            case INPUT_TYPE_SUBSHELL:
            {
                assert(false && "No INPUT_TYPE_SUBSHELL in execute_parallel");
                /*
                auto new_children{ fork_subshell(input.data.subshell) };
                for (auto&& i : new_children)
                {
                    children.emplace_back(i);
                }
                */
                break;
            }
            case INPUT_TYPE_COMMAND:
            {
                children.emplace_back(fork_command(input.data.cmd));
                break;
            }
            case INPUT_TYPE_PIPELINE:
            {
                auto new_children{ fork_pipeline(input.data.pline) };
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

pid_t eshell::fork_command(command& c) noexcept
{
    auto argv{ c.args }; // NOLINT
    assert(argv != nullptr);
    pid_t child{ fork() };
    if (child == 0) // child
    {
        execvp(argv[0], argv); // NOLINT
    }
    // parent
    return child;
}

pid_t eshell::fork_and_pipe(command& c,
                            const std::vector<fd>& pipes,
                            int i) noexcept
{
    auto argv{ c.args }; // NOLINT
    assert(argv != nullptr);
    pid_t child{ fork() };
    if (child == 0) // child
    {
        set_pipes(pipes, i);
        execvp(argv[0], argv); // NOLINT
    }
    // parent
    return child;
}

std::vector<pid_t> eshell::fork_subshell(char* sh) noexcept
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
            assert(p.num_inputs == 1);
            assert(p.inputs[0].type == INPUT_TYPE_COMMAND);
            children.emplace_back(fork());
            if (children.back() == 0)
            {
                // NOLINTNEXTLINE
                execvp(p.inputs[0].data.cmd.args[0], p.inputs[0].data.cmd.args);
            }
            break;
        }
        case SEPARATOR_PIPE:
        {
            auto new_children{ fork_pipeline(create_pipeline(p)) };
            for (auto&& i : new_children)
            {
                children.emplace_back(i);
            }
            break;
        }
        case SEPARATOR_SEQ:
        {
            for (int i{ 0 }; i < p.num_inputs; ++i)
            {
                assert(p.inputs[i].type == INPUT_TYPE_COMMAND);
                waitpid(fork_command(p.inputs[i].data.cmd), nullptr, 0);
            }
            break;
        }
        case SEPARATOR_PARA:
        {
            // create pipes
            std::vector<fd> pipes(p.num_inputs);
            for (int i{ 0 }; i < p.num_inputs; ++i)
            {
                pipes[i] = create_pipe();
            }
            // fork children
            for (int i{ 0 }; i < p.num_inputs; ++i)
            {
                children.emplace_back(fork());
                if (children.back() == 0) // child
                {
                    set_repeater_pipes(pipes, i);
                    assert(p.inputs[i].type == INPUT_TYPE_COMMAND);
                    // NOLINTNEXTLINE
                    auto* argv{ p.inputs[i].data.cmd.args };
                    // NOLINTNEXTLINE
                    execvp(argv[0], argv);
                }
            }
            // fork repeater
            children.emplace_back(fork());
            if (children.back() == 0) // repeater
            {
                repeater_procedure(pipes);
            }

            // close all pipes in parent
            for (auto&& i : pipes)
            {
                close(i.first);
                close(i.second);
            }
            break;
        }
    }
    return children;
}

std::vector<pid_t> eshell::fork_and_pipe_subshell(char* sh,
                                                  const std::vector<fd>& pipes,
                                                  int i) noexcept
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
            assert(p.num_inputs == 1);
            assert(p.inputs[0].type == INPUT_TYPE_COMMAND);
            children.emplace_back(fork());
            if (children.back() == 0)
            {
                set_pipes(pipes, i);
                // NOLINTNEXTLINE
                execvp(p.inputs[0].data.cmd.args[0], p.inputs[0].data.cmd.args);
            }
            break;
        }
        case SEPARATOR_PIPE:
        {
            // get input and output pipes
            auto input{ STDIN_FILENO };
            auto output{ STDOUT_FILENO };
            if (i > 0)
            {
                input = pipes[i - 1].first;
            }
            if (i < pipes.size())
            {
                output = pipes[i].second;
            }

            // create pipes
            assert(p.num_inputs > 0);
            std::vector<fd> new_pipes(p.num_inputs - 1);
            for (int i{ 0 }; i < p.num_inputs - 1; ++i)
            {
                new_pipes[i] = create_pipe();
            }
            // fork children
            for (int i{ 0 }; i < p.num_inputs; ++i)
            {
                children.emplace_back(fork());
                if (children.back() == 0) // child
                {
                    if (i > 0)
                    {
                        input = new_pipes[i - 1].first;
                    }
                    if (i < new_pipes.size())
                    {
                        output = new_pipes[i].second;
                    }
                    dup2(input, STDIN_FILENO);
                    dup2(output, STDOUT_FILENO);
                    for (auto&& j : pipes)
                    {
                        int input{ j.first };
                        close(input);
                        int output{ j.second };
                        close(output);
                    }
                    for (auto&& j : new_pipes)
                    {
                        int input{ j.first };
                        close(input);
                        int output{ j.second };
                        close(output);
                    }
                    assert(p.inputs[i].type == INPUT_TYPE_COMMAND);
                    // NOLINTNEXTLINE
                    auto* argv{ p.inputs[i].data.cmd.args };
                    // NOLINTNEXTLINE
                    execvp(argv[0], argv);
                }
            }
            // close all pipes in parent
            for (auto&& i : pipes)
            {
                close(i.first);
                close(i.second);
            }
            for (auto&& i : new_pipes)
            {
                close(i.first);
                close(i.second);
            }
            break;
        }
        case SEPARATOR_SEQ:
        {
            assert(false &&
                   "fork_and_pipe_subshell SEPARATOR_SEQ not implemented");
            break;
        }
        case SEPARATOR_PARA:
        {
            assert(false &&
                   "fork_and_pipe_subshell SEPARATOR_PARA not implemented");
            break;
        }
    }
    return children;
}

std::vector<pid_t> eshell::fork_pipeline(const pipeline& p) noexcept
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
            set_pipes(pipes, i);
            // NOLINTNEXTLINE
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

/*
std::vector<pid_t> eshell::fork_and_pipeline(pipeline& p,
                                             const std::vector<fd>& pipes,
                                             int i) noexcept
{
    ;
}
*/

pipeline eshell::create_pipeline(const parsed_input& p) noexcept
{
    assert(p.separator == SEPARATOR_PIPE);
    pipeline result;
    result.num_commands = p.num_inputs;
    assert(result.num_commands < MAX_INPUTS);
    for (int i{ 0 }; i < result.num_commands; ++i)
    {
        assert(p.inputs[i].type == INPUT_TYPE_COMMAND);
        result.commands[i] = p.inputs[i].data.cmd;
    }
    return result;
}
