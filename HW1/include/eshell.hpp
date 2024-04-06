#pragma once

#include "parser.h"

#include <cstdio>
#include <optional>

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
        inline static void print_prompt() noexcept
        {
            std::fputs("/> ", stdout);
        }
        static std::optional<parsed_input> get_input() noexcept;
        static bool process_input(std::optional<parsed_input>) noexcept;
        // NOLINTNEXTLINE (*-avoid-c-arrays)
        static void execute(char* const[MAX_ARGS]) noexcept;
        static bool is_quit(char*) noexcept;
};
