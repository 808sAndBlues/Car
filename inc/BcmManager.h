#ifndef BCM_MANAGER_H
#define BCM_MANAGER_H

#include <bcm2835.h>
#include <cstdint>
#include <sys/timerfd.h>

#include "Logger.h"
#include "KillFlag.h"
#include "Epoll.h"

#define GPIO_COUNT 26

const RPiGPIOPin GPIO_PINS[] = 
{
    RPI_BPLUS_GPIO_J8_03, RPI_BPLUS_GPIO_J8_05, RPI_BPLUS_GPIO_J8_07,
    RPI_BPLUS_GPIO_J8_08, RPI_BPLUS_GPIO_J8_10, RPI_BPLUS_GPIO_J8_11,
    RPI_BPLUS_GPIO_J8_12, RPI_BPLUS_GPIO_J8_13, RPI_BPLUS_GPIO_J8_15,
    RPI_BPLUS_GPIO_J8_16, RPI_BPLUS_GPIO_J8_18, RPI_BPLUS_GPIO_J8_19,
    RPI_BPLUS_GPIO_J8_21, RPI_BPLUS_GPIO_J8_22, RPI_BPLUS_GPIO_J8_23,
    RPI_BPLUS_GPIO_J8_24, RPI_BPLUS_GPIO_J8_26, RPI_BPLUS_GPIO_J8_29,
    RPI_BPLUS_GPIO_J8_31, RPI_BPLUS_GPIO_J8_32, RPI_BPLUS_GPIO_J8_33,
    RPI_BPLUS_GPIO_J8_35, RPI_BPLUS_GPIO_J8_36, RPI_BPLUS_GPIO_J8_37,
    RPI_BPLUS_GPIO_J8_38, RPI_BPLUS_GPIO_J8_40 
};

class BcmManager
{
    protected:
        Logger& _logger;
        KillFlag& _kill_flag;
        Epoll _epoll;

        int _timerfd = 0;

        void setup_timerfd();

        void poll_events();

        void evaluate_events();

    public:
        BcmManager(Logger& logger) : _logger(logger) {}

        ~BcmManager();

        void init();

        void main_loop();

        void set_pin(RPiGPIOPin pin);

        void set_io_state(RPiGPIOPin physical_pin, bcm2835FunctionSelect mode);

        void clear_pin(RPiGPIOPin physical_pin);
};

void* bcm_manager_main_loop(void* obj);

#endif
