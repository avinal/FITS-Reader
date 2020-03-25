#include "fits.hpp"

int main(int argc, char const *argv[])
{
    std::string to_read;
    std::cout << "Enter file name with extension e.g. 'file.fits'" << std::endl;
    std::cin >> to_read;
    readfits check(to_read);

    /*
Creates a map of functions to execute the commands like 'display' and 'update'.
*/
    std::map<std::string, std::function<void(std::string)>> instruction;
    instruction["update"] = [&check](std::string in) { check.update_headers(in); };
    instruction["display"] = [&check](std::string in) { check.display(in); };

    std::cout << "Enter the command followed by arguments, enter 'end' to exit" << std::endl;
    std::string command, argument;
    std::cin >> command;

    while (command.compare("end"))
    {
        std::cin >> argument;
        boost::to_upper(argument);
        instruction[command](argument);
        std::cin >> command;
    }
    return 0;
}
