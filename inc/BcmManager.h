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
#include "Server.h"

#define INIT_LED_GPIO_PIN         RPI_BPLUS_GPIO_J8_07
#define ADVENTURE_LED_GPIO_PIN    RPI_BPLUS_GPIO_J8_40

// Motor-related pins
#define MOTOR_A_FWD_PIN           RPI_BPLUS_GPIO_J8_16
#define MOTOR_A_REVERSE_PIN       RPI_BPLUS_GPIO_J8_11
#define MOTOR_B_FWD_PIN           RPI_BPLUS_GPIO_J8_26
#define MOTOR_B_REVERSE_PIN       RPI_BPLUS_GPIO_J8_15
#define MOTOR_A_PWM_PIN           RPI_BPLUS_GPIO_J8_12
#define MOTOR_B_PWM_PIN           RPI_BPLUS_GPIO_J8_32

// PWM-related defines
#define TARGET_PWM_FRQ_HZ         16000
#define CLOCK_FREQ_HZ             19200000
#define WRAP                      (CLOCK_FREQ_HZ/TARGET_PWM_FRQ_HZ) - 1 
#define CLOCK_DIV                 CLOCK_FREQ_HZ/(TARGET_PWM_FRQ_HZ * WRAP)
#define DUTY_RATIO                0.8
#define PWM_CHANNEL_0             0

#define CONTROL_POLL_NS           100000000
#define RECV_SIZE                 255

// Control defines
#define FORWARD_CTL_VAL           0
#define REVERSE_CTL_VAL           2
#define DEFAULT_CTL_VAL           99

const std::uint8_t RECV_BUFFER  = RECV_SIZE;
const std::uint8_t DFLT_CTL_VAL = DEFAULT_CTL_VAL;

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
        Server _server;

        GPIOStatus _gpio_status;
        TimeStatus _time_status;
        MotorStatus _motor_status;

        std::uint8_t _recv_buffer[RECV_SIZE] = {0};

        int _tlm_timerfd = 0;
        int _control_timerfd = 0;
        
        std::uint8_t _received_ctl_val = 0;

        // Polling functions
        void poll_events();

        void evaluate_events(int fds);

        // Setup functions
        void setup_tlm_timerfd();
        
        void setup_control_timerfd();

        void setup_motor_a();

        void setup_motor_b();

        void setup_epoll();

        void setup_motors();

        void setup_pwm_pins();

        void setup_pwm();

        void set_adventure_led();

        void set_pin(RPiGPIOPin pin);

        void clear_pin(RPiGPIOPin physical_pin);

        // Send functions
        void send_telemetry();

        void send_gpio_status();

        void send_motor_status();

        void send_time_status();

        // Handler functions
        void tlm_timer_handler();

        void control_timer_handler();

        // Serialize functions
        void serialize_gpio_status(std::uint8_t* buf, int length);

        void serialize_motor_status(std::uint8_t* buf, size_t size);

        void serialize_time_status(std::uint8_t* buf, int length);

        // Update functions
        void update_gpio_status();

        void update_motor_status();

        void update_motor_a_state();

        void update_motor_b_state();

        void update_time_status();
        
        // Process functions
        void process_control_data(int received);

        void shutdown_pwm();

        void motor_a_forward();

        void motor_b_forward();

        void drive_backward(std::uint16_t data);

        void motor_a_backward();

        void motor_b_backward();

        void drive_forward(std::uint16_t data);

        // Info functions
        std::uint16_t compute_wrap();

        std::uint8_t get_level(RPiGPIOPin pin);

        // Closing fuctions
        void close_adventure_led();

        void shutdown_sequence();

    public:
        BcmManager(Logger& logger, KillFlag& kill_flag);

        ~BcmManager();

        void init();

        void main_loop();

};

void* bcm_manager_main_loop(void* obj);

#endif
