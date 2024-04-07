#pragma once

#include "parser.h"

#include <cstdio>
#include <optional>
#include <sys/types.h>
#include <utility>
#include <vector>

class eshell
{
    public:
        eshell() noexcept
        {
            do // NOLINT (cppcoreguidelines-avoid-do-while)
            {
                print_prompt();
            } while (process_input(get_input()));
        }

    private:
        using fd = std::pair<int, int>;
        inline static void print_prompt() noexcept
        {
            std::fputs("/> ", stdout);
        }
        static std::optional<parsed_input> get_input() noexcept;
        static bool process_input(std::optional<parsed_input>) noexcept;
        static bool is_quit(char*) noexcept;
        static std::pair<int, int> create_pipe() noexcept;
        static void set_pipes(const std::vector<fd>&, int) noexcept;

        static void execute_single(parsed_input&) noexcept;
        static void execute_pipeline(parsed_input&) noexcept;
        static void execute_sequential(parsed_input&) noexcept;
        static void execute_parallel(parsed_input&) noexcept;

        static pid_t fork_command(command&) noexcept;
        static pid_t fork_and_pipe(command&,
                                   auto&&,
                                   const std::vector<fd>&,
                                   int) noexcept;
        static std::vector<pid_t> fork_subshell(char*) noexcept;
        static std::vector<pid_t> fork_and_pipe_subshell(char*,
                                                         auto&&,
                                                         const std::vector<fd>&,
                                                         int) noexcept;
        static std::vector<pid_t> fork_pipeline(pipeline&) noexcept;
};
