#pragma once

#include "parser.h"

#include <cstdio>
#include <string>
#include <sys/types.h>
#include <utility>
#include <vector>

class eshell
{
    public:
        eshell() noexcept;

    private:
        using fd = std::pair<int, int>;
        inline static void print_prompt() noexcept
        {
            std::fputs("/> ", stdout);
        }
        static char* get_input(std::string& str);
        [[noreturn]] static void subshell(char* sh, bool is_toplevel = false);

        static void execute_single(parsed_input&) noexcept;
        static void execute_pipeline(parsed_input&) noexcept;
        static void execute_sequential(parsed_input&) noexcept;
        static void execute_parallel(parsed_input&) noexcept;
        static void execute_parallel_with_repeater(parsed_input&) noexcept;

        static pid_t fork_command(command&) noexcept;
        static void wait_command(command&) noexcept;
        [[noreturn]] static void execute_command(command&) noexcept;

        static std::vector<pid_t> fork_pipeline(const pipeline&) noexcept;
        static std::vector<pid_t> fork_pipeline_for_repeater(
          const pipeline&,
          const std::vector<fd>&,
          std::size_t) noexcept;

        static std::pair<int, int> create_pipe() noexcept;
        static void set_pipes(const std::vector<fd>&, std::size_t) noexcept;
        static void set_repeater_pipes(const std::vector<fd>&,
                                       std::size_t) noexcept;
        [[noreturn]] static void repeater_procedure(std::vector<fd>) noexcept;
};
