#include <iostream>
#include <unistd.h>
#include <pthread.h>
#include <signal.h>
#include <atomic>

#include "Logger.h"
#include "Signal.h"

const char* OPT_STRING = "d::";
KillFlag kill_flag;


int main(int argc, char* argv[])
{
    std::int16_t c = 0;
    bool debug_mode = false;


    // TODO: Store pthread_t values into array for easier creation/deletion
    pthread_t log_tid = 0;

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

    Logger logger(kill_flag, debug_mode);
    Signal signal(logger, kill_flag);

    signal.init();

    if (pthread_create(&log_tid, nullptr, logger_main,
                       &logger) == -1) {
        std::cout << "Error creating thread\n";
        std::perror("pthread_create");
        std::exit(-1);
    }
    
    // TODO: Do I really need a ret value on this? 
    void* ret = nullptr;
    if (pthread_join(log_tid, &ret) == -1) {
        std::cout << "Error joining thread\n";
        std::perror("pthread_join");
        std::exit(-1);
    }


    return 0;
}

