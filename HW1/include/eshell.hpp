#pragma once

#include "parser.h"
#include <cstdio>

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
        static parsed_input get_input() noexcept;
        static bool process_input(parsed_input) noexcept;
        static bool process_one_input(char* const[]) noexcept;
        static bool is_quit(char* const) noexcept;
};
