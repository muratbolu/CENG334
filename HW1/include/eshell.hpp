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
        // NOLINTNEXTLINE (*-avoid-c-arrays)
        static pid_t execute_command(command&) noexcept;
        static bool is_quit(char*) noexcept;
        static std::pair<int, int> create_pipe() noexcept;
        static void execute_single(parsed_input&) noexcept;
        static std::vector<pid_t> execute_subshell(char*) noexcept;
        static std::vector<pid_t> execute_pipeline(parsed_input&) noexcept;
        static std::vector<pid_t> execute_pipeline(pipeline&) noexcept;
        static std::vector<pid_t> inner_pipeline_member() noexcept;
        static void execute_sequential(parsed_input&) noexcept;
        static void execute_parallel(parsed_input&) noexcept;
};
