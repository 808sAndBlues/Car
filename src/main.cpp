#include <iostream>
#include <unistd.h>

const char* OPT_STRING = "d::";

int main(int argc, char* argv[])
{
    std::int16_t c = 0;
    bool debug_mode = false;

    while ((c = getopt(argc, argv, OPT_STRING)) != -1) {
        switch (c) {
            case 'd':
                std::cout << "Debug mode enabled\n";
                debug_mode = true;
                break;

            default:
                std::cout << "Unhandled param\n";
                std::exit(-1);
                break;
        }
    }

    return 0;
        
}
