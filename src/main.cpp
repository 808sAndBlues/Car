#include <iostream>
#include <unistd.h>
#include <pthread.h>
#include <signal.h>
#include <atomic>
#include <bcm2835.h>

#include "Logger.h"
#include "Signal.h"
#include "BcmManager.h"
#include "Client.h"

const char* OPT_STRING = "d::";
KillFlag kill_flag;

int main(int argc, char* argv[])
{
    std::int16_t c = 0;
    bool debug_mode = false;

    // TODO: Store pthread_t values into array for easier creation/deletion
    pthread_t log_tid = 0;
    pthread_t signal_tid = 0;
    pthread_t bcm_tid = 0;
    pthread_t client_tid = 0;

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
    logger.init();

    Signal signal(logger, kill_flag);
    signal.init();

    BcmManager bcm_manager(logger, kill_flag);
    bcm_manager.init();

    Client client(logger, kill_flag);
    client.init();

    if (pthread_create(&bcm_tid, nullptr, bcm_manager_main_loop, &bcm_manager) == -1) {
        std::cout << "Error creating thread for bcm\n";
        std::perror("pthread_crate\n");
        std::exit(-1);
    }

    if (pthread_create(&log_tid, nullptr, logger_main,
                       &logger) == -1) {
        std::cout << "Error creating thread\n";
        std::perror("pthread_create");
        std::exit(-1);
    }
    
    if (pthread_create(&signal_tid, nullptr, signal_main_loop, &signal) == -1) {
        std::cout << "Error creating thread for signal\n";
        std::perror("pthread_crate\n");
        std::exit(-1);
    }
    
    if (pthread_create(&client_tid, nullptr, client_main_loop, &client) == -1) {
        std::cout << "Error creating thread for client loop\n";
        std::perror("pthread_crate\n");
        std::exit(-1);
    }
    
    if (pthread_join(bcm_tid, nullptr) == -1) {
        std::cout << "Error joining thread\n";
        std::perror("pthread_join");
        std::exit(-1);
    }

    if (pthread_join(signal_tid, nullptr) == -1) {
        std::cout << "Error joining thread\n";
        std::perror("pthread_join");
        std::exit(-1);
    }

    if (pthread_join(log_tid, nullptr) == -1) {
        std::cout << "Error joining thread\n";
        std::perror("pthread_join");
        std::exit(-1);
    }

    if (pthread_join(client_tid, nullptr) == -1) {
        std::cout << "Error joining thread\n";
        std::perror("pthread_join");
        std::exit(-1);
    }

    return 0;
}

