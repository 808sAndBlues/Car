#ifndef BCM_MANAGER_H
#define BCM_MANAGER_H

#include <bcm2835.h>
#include <cstdint>
#include <sys/timerfd.h>
#include <endian.h>

#include "Logger.h"
#include "KillFlag.h"
#include "Epoll.h"
#include "Client.h"
#include "Telemetry.h"

#define INIT_LED_GPIO_PIN       RPI_BPLUS_GPIO_J8_07
#define ADVENTURE_LED_GPIO_PIN  RPI_BPLUS_GPIO_J8_40

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
        Client _client;

        GPIOStatus _gpio_status;
        TimeStatus _time_status;

        int _timerfd = 0;

        void setup_timerfd();

        void set_init_led();

        void close_init_led();

        void set_adventure_led();
        
        void close_adventure_led();

        void poll_events();

        void evaluate_events(int fds);

        void step(int val);

        void setup_epoll();

        void timer_handler();

        void send_gpio_status();

        void serialize_gpio_status(std::uint8_t* buf, int length);

        void update_gpio_status();

        void send_time_status();
        
        void serialize_time_status(std::uint8_t* buf, int length);

        void update_time_status();

    public:
        BcmManager(Logger& logger, KillFlag& kill_flag);

        ~BcmManager();

        void init();

        void main_loop();

        void set_pin(RPiGPIOPin pin);

        void set_io_state(RPiGPIOPin physical_pin, bcm2835FunctionSelect mode);

        void clear_pin(RPiGPIOPin physical_pin);
};

void* bcm_manager_main_loop(void* obj);

#endif
