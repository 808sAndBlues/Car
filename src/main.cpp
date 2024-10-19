#include <iostream>
#include <unistd.h>
#include <pthread.h>
#include <signal.h>
#include <atomic>

#include "Logger.h"

const char* OPT_STRING = "d::";

void setup_sigmask()
{
    sigset_t signal_set;
    if (sigemptyset(&signal_set) == -1) {
        std::cout << "Error filling w/ empty set\n";
        std::perror("sigemptyset");
        std::exit(-1);
    }

    if (sigaddset(&signal_set, SIGTERM) == -1) {
        std::cout << "Failed to add SIGTERM to signal set\n";
        std::perror("sigaddset");
        std::exit(-1);
    }

    if (sigaddset(&signal_set, SIGINT) == -1) {
        std::cout << "Failed to add SIGINT to signal set\n";
        std::perror("sigaddset");
        std::exit(-1);
    }

    if (sigprocmask(SIG_UNBLOCK, &signal_set, 0) == -1) {
        std::cout << "Failed to unblock signal set\n";
        std::perror("sigprocmask");
        std::exit(-1);
    }

    // TODO: Update this w/ Log statements
    std::cout << "Ready to catch SIGTERM and SIGINT\n";
}

int main(int argc, char* argv[])
{
    std::int16_t c = 0;
    bool debug_mode = false;

    KillFlag kill_flag;


    
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

    setup_sigmask();

    Logger logger(kill_flag);

    if (pthread_create(&log_tid, nullptr, logger_main,
                       &logger) == -1) {
        std::cout << "Error creating thread\n";
        std::perror("pthread_create");
        std::exit(-1);
    }
    
    void* ret = nullptr;
    if (pthread_join(log_tid, &ret)) {
        std::cout << "Error joining thread\n";
        std::perror("pthread_join");
        std::exit(-1);
    }

    return 0;
}

