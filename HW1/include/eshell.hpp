#pragma once

#include "parser.h"

#include <cstdio>
#include <optional>
#include <sys/types.h>
#include <utility>

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
        static pid_t spawn(char* const[MAX_ARGS]) noexcept;
        static bool is_quit(char*) noexcept;
        static std::pair<int, int> create_pipe() noexcept;
        static void execute_pipeline(const parsed_input&) noexcept;
};
