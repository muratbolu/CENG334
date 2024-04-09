#include "eshell.hpp"
#include "parser.h"

#include <array>
#include <cassert>
#include <cstddef>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <stdexcept>
#include <string>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <utility>
#include <vector>

eshell::eshell() noexcept
{
    for (;;)
    {
        print_prompt();
        std::string str;
        try
        {
            char* i{ get_input(str) };
            pid_t f{ fork() };
            if (f == 0) // NOLINT (child)
            {
                subshell(i, true);
            }
            waitpid(f, nullptr, 0);
        }
        catch (const std::runtime_error&) // EOF, quit
        {
            break;
        }
        catch (const std::invalid_argument&) // unparseable
        {
            continue;
        }
    }
}

char* eshell::get_input(std::string& str)
{
    std::getline(std::cin, str);
    if (std::cin.eof())
    {
        throw std::runtime_error{ "EOF" };
    }
    if (str == "quit")
    {
        throw std::runtime_error{ "quit" };
    }
    return str.data();
}

// NOLINTNEXTLINE(*-no-recursion)
[[noreturn]] void eshell::subshell(char* sh, bool is_toplevel)
{
    parsed_input p;
    if (parse_line(sh, &p) == 0)
    {
        throw std::invalid_argument{ "Unparseable input" };
    }
    // DO NOT CHANGE STDIN, STDOUT, caller is responsible!
    switch (p.separator)
    {
        case SEPARATOR_NONE:
        {
            // only one input for no separator case
            assert(p.num_inputs == 1);
            // either single command or subshell
            execute_single(p);
            break;
        }
        case SEPARATOR_PIPE:
        {
            // at least one input
            assert(p.num_inputs > 0);
            execute_pipeline(p);
            break;
        }
        case SEPARATOR_SEQ:
        {
            // at least one input
            assert(p.num_inputs > 0);
            execute_sequential(p);
            break;
        }
        case SEPARATOR_PARA:
        {
            // at least one input
            assert(p.num_inputs > 0);
            // if the subshell is not top level, it needs repeater
            if (is_toplevel)
            {
                execute_parallel(p);
            }
            else
            {
                execute_parallel_with_repeater(p);
            }
            break;
        }
    }
    free_parsed_input(&p);
    exit(EXIT_SUCCESS); // NOLINT
}

// NOLINTNEXTLINE
void eshell::execute_single(parsed_input& p) noexcept
{
    single_input input{ p.inputs[0] };
    switch (input.type)
    {
        case INPUT_TYPE_NON:
        {
            assert(false && "No INPUT_TYPE_NON in execute_single");
            break;
        }
        case INPUT_TYPE_SUBSHELL:
        {
            pid_t f{ fork() };
            if (f == 0) // NOLINT (child)
            {
                // NOLINTNEXTLINE
                subshell(input.data.subshell);
            }
            waitpid(f, nullptr, 0);
            break;
        }
        case INPUT_TYPE_COMMAND:
        {
            wait_command(input.data.cmd);
            break;
        }
        case INPUT_TYPE_PIPELINE:
        {
            assert(false && "No INPUT_TYPE_PIPELINE in execute_single");
            break;
        }
    }
}

// NOLINTNEXTLINE
void eshell::execute_pipeline(parsed_input& p) noexcept
{
    std::size_t num_inputs{ static_cast<std::size_t>(p.num_inputs) };
    std::vector<pid_t> children;
    std::size_t num_pipes{ static_cast<std::size_t>(p.num_inputs - 1) };
    std::vector<fd> pipes(num_pipes);

    // initialize all pipes
    for (std::size_t i{ 0 }; i < num_pipes; ++i)
    {
        pipes[i] = create_pipe();
    }

    // spawn children
    for (std::size_t i{ 0 }; i < num_inputs; ++i)
    {
        single_input input{ p.inputs[i] }; // NOLINT
        switch (input.type)
        {
            case INPUT_TYPE_NON:
            {
                assert(false && "No INPUT_TYPE_NON in execute_pipeline");
                break;
            }
            case INPUT_TYPE_SUBSHELL:
            {
                children.emplace_back(fork());
                if (children.back() == 0) // child
                {
                    set_pipes(pipes, i);
                    // NOLINTNEXTLINE
                    subshell(input.data.subshell);
                }
                break;
            }
            case INPUT_TYPE_COMMAND:
            {
                children.emplace_back(fork());
                if (children.back() == 0) // child
                {
                    set_pipes(pipes, i);
                    // NOLINTNEXTLINE
                    execute_command(input.data.cmd);
                }
                break;
            }
            case INPUT_TYPE_PIPELINE:
            {
                assert(false && "No INPUT_TYPE_PIPELINE in execute_pipeline");
                break;
            }
        }
    }
    for (auto&& s : pipes)
    {
        close(s.first);
        close(s.second);
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
        single_input input{ p.inputs[i] }; // NOLINT
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
                break;
            }
            case INPUT_TYPE_COMMAND:
            {
                wait_command(input.data.cmd);
                break;
            }
            case INPUT_TYPE_PIPELINE:
            {
                auto new_children{ fork_pipeline(input.data.pline) };
                for (auto&& c : new_children)
                {
                    waitpid(c, nullptr, 0);
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
        single_input input{ p.inputs[i] }; // NOLINT
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
                for (auto&& c : new_children)
                {
                    children.emplace_back(c);
                }
                break;
            }
        }
    }
    for (auto&& c : children)
    {
        waitpid(c, nullptr, 0);
    }
}

// TODO
void eshell::execute_parallel_with_repeater(parsed_input& p) noexcept
{
    std::size_t num_inputs{ static_cast<std::size_t>(p.num_inputs) };
    std::vector<pid_t> children;
    std::size_t num_pipes{ static_cast<std::size_t>(p.num_inputs) };
    std::vector<fd> pipes(num_pipes);

    // initialize all pipes
    for (std::size_t i{ 0 }; i < num_pipes; ++i)
    {
        pipes[i] = create_pipe();
    }

    // spawn children
    for (std::size_t i{ 0 }; i < num_inputs; ++i)
    {
        single_input input{ p.inputs[i] }; // NOLINT
        switch (input.type)
        {
            case INPUT_TYPE_NON:
            {
                assert(0 && "No INPUT_TYPE_NON in e_p_w_r");
                break;
            }
            case INPUT_TYPE_SUBSHELL:
            {
                assert(0 && "No INPUT_TYPE_SUBSHELL in e_p_w_r");
                break;
            }
            case INPUT_TYPE_COMMAND:
            {
                children.emplace_back(fork());
                if (children.back() == 0) // child
                {
                    set_repeater_pipes(pipes, i);
                    // NOLINTNEXTLINE
                    execute_command(input.data.cmd);
                }
                break;
            }
            case INPUT_TYPE_PIPELINE:
            {
                // TODO
                assert(0 && "TODO");
                /*
                children.emplace_back(fork());
                if (children.back() == 0) // child
                {
                    set_repeater_pipes(pipes, i);
                    // NOLINTNEXTLINE
                    execute_command(input.data.cmd);
                }
                break;

                auto new_children{ fork_pipeline(input.data.pline) };
                for (auto&& c : new_children)
                {
                    children.emplace_back(c);
                }
                break;
                */
            }
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

    for (auto&& c : children)
    {
        waitpid(c, nullptr, 0);
    }
}

pid_t eshell::fork_command(command& c) noexcept
{
    auto argv{ c.args }; // NOLINT
    pid_t child{ fork() };
    if (child == 0) // child
    {
        execvp(argv[0], argv); // NOLINT
    }
    // parent
    return child;
}

void eshell::wait_command(command& c) noexcept
{
    waitpid(fork_command(c), nullptr, 0);
}

[[noreturn]] void eshell::execute_command(command& c) noexcept
{
    auto argv{ c.args };   // NOLINT
    execvp(argv[0], argv); // NOLINT
} // NOLINT

std::vector<pid_t> eshell::fork_pipeline(const pipeline& p) noexcept
{
    std::size_t num_commands{ static_cast<std::size_t>(p.num_commands) };
    std::vector<pid_t> children;
    std::size_t num_pipes{ static_cast<std::size_t>(p.num_commands - 1) };
    std::vector<fd> pipes(num_pipes);

    // initialize all pipes
    for (std::size_t i{ 0 }; i < num_pipes; ++i)
    {
        pipes[i] = create_pipe();
    }

    // spawn children
    for (std::size_t i{ 0 }; i < num_commands; ++i)
    {
        children.emplace_back(fork());
        if (children.back() == 0) // child
        {
            set_pipes(pipes, i);
            // NOLINTNEXTLINE
            execvp(p.commands[i].args[0], p.commands[i].args);
        }
    }
    for (auto&& s : pipes)
    {
        close(s.first);
        close(s.second);
    }
    return children;
}

eshell::fd eshell::create_pipe() noexcept
{
    std::array<int, 2> fd;
    pipe(fd.data());
    return std::make_pair(fd[0], fd[1]);
}

void eshell::set_pipes(const std::vector<fd>& pipes, std::size_t i) noexcept
{
    // set input, if applicable
    if (i > 0)
    {
        int inp{ pipes[i - 1].first };
        dup2(inp, STDIN_FILENO);
    }

    // set output, if applicable
    if (i < pipes.size())
    {
        int out{ pipes[i].second };
        dup2(out, STDOUT_FILENO);
    }

    // close all other pipes
    for (auto&& j : pipes)
    {
        int inp{ j.first };
        close(inp);
        int out{ j.second };
        close(out);
    }
}

void eshell::set_repeater_pipes(const std::vector<fd>& pipes,
                                std::size_t i) noexcept
{
    // set input
    int inp{ pipes[i].first };
    dup2(inp, STDIN_FILENO);

    // do not set output

    // close all other pipes
    for (auto&& j : pipes)
    {
        int in{ j.first };
        close(in);
        int out{ j.second };
        close(out);
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
            if (write(pipes[i].second,
                      buffer.data(),
                      static_cast<std::size_t>(n)) == 0)
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
    exit(EXIT_SUCCESS); // NOLINT
}

/*
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
            execute_sequential(p);
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
                switch (p.inputs[i].type)
                {
                    case INPUT_TYPE_NON:
                    {
                        assert(false && "No INPUT_TYPE_NON in "
                                        "SEPARATOR_PARA in fork_subshell");
                        break;
                    }
                    case INPUT_TYPE_SUBSHELL:
                    {
                        assert(false && "No INPUT_TYPE_SUBSHELL in "
                                        "SEPARATOR_PARA in fork_subshell");
                        break;
                    }
                    case INPUT_TYPE_COMMAND:
                    {
                        children.emplace_back(fork());
                        if (children.back() == 0) // child
                        {
                            set_repeater_pipes(pipes, i);
                            // NOLINTNEXTLINE
                            auto* argv{ p.inputs[i].data.cmd.args };
                            // NOLINTNEXTLINE
                            execvp(argv[0], argv);
                        }
                        break;
                    }
                    case INPUT_TYPE_PIPELINE:
                    {
                        auto new_children{ fork_and_pipeline(
                          p.inputs[i].data.pline, pipes, i) };
                        for (auto&& i : new_children)
                        {
                            children.emplace_back(i);
                        }
                    }
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

void eshell::fork_and_pipe_subshell(char* sh,
                                    const std::vector<fd>& pipes,
                                    int j) noexcept
{
    parsed_input p;
    if (parse_line(sh, &p) == 0)
    {
        assert(false && "Parse error in execute_subshell");
    }
    // pretty_print(&p);
    SEPARATOR sep{ p.separator };
    switch (sep)
    {
        case SEPARATOR_NONE:
        {
            assert(p.num_inputs == 1);
            assert(p.inputs[0].type == INPUT_TYPE_COMMAND);
            waitpid(fork_and_pipe(p.inputs[0].data.cmd, pipes, j), nullptr, 0);
            break;
        }
        case SEPARATOR_PIPE:
        {
            // get input and output pipes
            int input{ STDIN_FILENO };
            int output{ STDOUT_FILENO };
            if (j > 0)
            {
                input = pipes[j - 1].first;
            }
            if (j < pipes.size())
            {
                output = pipes[j].second;
            }

            // create pipes
            assert(p.num_inputs > 0);
            std::vector<pid_t> children;
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
            for (auto&& i : new_pipes)
            {
                close(i.first);
                close(i.second);
            }
            for (auto&& i : children)
            {
                waitpid(i, nullptr, 0);
            }
            break;
        }
        case SEPARATOR_SEQ:
        {
            assert(p.num_inputs > 0);
            for (int i{ 0 }; i < p.num_inputs; ++i)
            {
                switch (p.inputs[i].type)
                {
                    case INPUT_TYPE_NON:
                    {
                        assert(false && "No INPUT_TYPE_NON in SEPARATOR_SEQ in "
                                        "fork_and_pipe_subshell");
                        break;
                    }
                    case INPUT_TYPE_SUBSHELL:
                    {
                        assert(false &&
                               "No INPUT_TYPE_SUBSHELL in SEPARATOR_SEQ in "
                               "fork_and_pipe_subshell");
                        break;
                    }
                    case INPUT_TYPE_COMMAND:
                    {
                        waitpid(fork_command(p.inputs[i].data.cmd), nullptr, 0);
                        break;
                    }
                    case INPUT_TYPE_PIPELINE:
                    {
                        auto new_children{ fork_pipeline(
                          p.inputs[i].data.pline) };
                        for (auto&& i : new_children)
                        {
                            waitpid(i, nullptr, 0);
                        }
                        break;
                    }
                }
            }
            break;
        }
        case SEPARATOR_PARA:
        {
            assert(false &&
                   "fork_and_pipe_subshell SEPARATOR_PARA not implemented");
            break;
        }
    }
}

std::vector<pid_t> eshell::fork_and_pipeline(pipeline& p,
                                             const std::vector<fd>& pipes,
                                             int j) noexcept
{
    std::vector<pid_t> children;
    std::vector<fd> new_pipes(p.num_commands - 1);

    // initialize all pipes
    for (int i{ 0 }; i < p.num_commands - 1; ++i)
    {
        new_pipes[i] = create_pipe();
    }

    // spawn children
    for (int i{ 0 }; i < p.num_commands; ++i)
    {
        children.emplace_back(fork());
        if (children.back() == 0) // child
        {
            int input{ STDIN_FILENO };
            if (i > 0)
            {
                input = new_pipes[i - 1].first;
            }
            else // i == 0
            {
                input = pipes[j].first;
            }
            dup2(input, STDIN_FILENO);

            int output{ STDOUT_FILENO };
            if (i < static_cast<int>(new_pipes.size()))
            {
                output = new_pipes[i].second;
            }
            dup2(output, STDOUT_FILENO);

            // close all other pipes
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
            // NOLINTNEXTLINE
            execvp(p.commands[i].args[0], p.commands[i].args);
        }
    }
    for (int i{ 0 }; i < p.num_commands - 1; ++i)
    {
        close(new_pipes[i].first);
        close(new_pipes[i].second);
    }
    return children;
}

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
*/
