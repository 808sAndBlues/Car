#include <iostream>
#include <unistd.h>
#include <pthread.h>
#include <signal.h>
#include <atomic>
#include <bcm2835.h>

#include "Logger.h"
#include "Signal.h"

#define GPIO_LEVEL_ADDRESS_0 0x3F200034
#define GPIO_LEVEL_ADDRESS_1 0x3F200038
#define GPIO_LEVEL_0_MAX     31
#define GPIO_COUNT           54

const char* OPT_STRING = "d::";
KillFlag kill_flag;



int main(int argc, char* argv[])
{
    std::int16_t c = 0;
    bool debug_mode = false;

    if (!bcm2835_init()) {
        std::cout << "Failed to init bcm2385 module\n";
        std::exit(-1);
    }



    // TODO: Store pthread_t values into array for easier creation/deletion
    pthread_t log_tid = 0;
    pthread_t signal_tid = 0;

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

    // TODO: Light up LED if all is good
    bcm2835_gpio_fsel(RPI_BPLUS_GPIO_J8_36, BCM2835_GPIO_FSEL_OUTP);
    bcm2835_gpio_set(RPI_BPLUS_GPIO_J8_36);

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

    
    // TODO: Do I really need a ret value on this? 
    void* ret = nullptr;
    if (pthread_join(log_tid, &ret) == -1) {
        std::cout << "Error joining thread\n";
        std::perror("pthread_join");
        std::exit(-1);
    }


    if (pthread_join(signal_tid, &ret) == -1) {
        std::cout << "Error joining thread\n";
        std::perror("pthread_join");
        std::exit(-1);
    }


    bcm2835_gpio_clr(RPI_BPLUS_GPIO_J8_36);

    return 0;
}

