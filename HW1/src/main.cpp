#include "parser.h"

#include <cstdlib>
#include <cstring>
#include <iostream>
#include <memory>
#include <string>
#include <vector>

int main()
{
    for (;;)
    {
        std::cout << "/> ";
        std::string str;
        std::getline(std::cin, str, '\n');
        std::unique_ptr<parsed_input> parsed = std::make_unique<parsed_input>();
        std::vector<char> input(str.cbegin(), str.cend());
        parse_line(input.data(), parsed.get());
        // pretty_print(parsed.get());
        if (parsed->num_inputs == 1 &&
            parsed->inputs[0].type == INPUT_TYPE_COMMAND &&
            std::strcmp(*(parsed->inputs[0].data.cmd.args), "quit") == 0)
        {
            std::exit(EXIT_SUCCESS);
        }
    }
    return 0;
}
