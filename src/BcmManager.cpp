#include "BcmManager.h"

void BcmManager::init()
{
    _epoll.init();

    if (!bcm2835_init()) {
        _logger.log_debug("Failed to init BCM2385 lbirary");
        std::perror("bcm2835.init()");
        std::exit(-1);
    }

    // Clear all pins
    for (std::uint8_t i = 0; i < GPIO_COUNT; ++i) {
        bcm2835_gpio_clr(GPIO_PINS[i]); 
    }

    set_io_state(RPI_BPLUS_GPIO_J8_36, BCM2835_GPIO_FSEL_OUTP);
    set_pin(RPI_BPLUS_GPIO_J8_36);
}

void BcmManager::setup_timerfd()
{
    _timerfd = timerfd_create(CLOCK_BOOTTIME, 0);

    struct itimerspec timer_spec = {0};
    timer_spec.it_value.sec = 1;
    timer_sepc.it_value.nsec = 0;
    timer_spec.it_interval.sec = 1;
    timer_spec.it_interval.nsec = 0;

    _epoll.add_fd(_timerfd);
}

void BcmManager::poll_events()
{
    int fds = _epoll.poll_events(0);
}

void BcmManager::evaluate_events(int fds)
{
    struct epoll_event* events = _epoll.get_events();

    for (int i = 0; i < fds; ++i) {
        if (events[i].data.fd == _timerfd) {
            // TODO: Call timer handler
        }
    }
}

BcmManager::~BcmManager()
{
    // TODO: Check return code
    clear_pin(RPI_BPLUS_GPIO_J8_36);
    bcm2835_close();
}

void BcmManager::set_io_state(RPiGPIOPin pin, bcm2835FunctionSelect mode)
{
    bcm2835_gpio_fsel(pin, mode);
}

void BcmManager::set_pin(RPiGPIOPin physical_pin)
{
    bcm2835_gpio_set(physical_pin);
}

void BcmManager::clear_pin(RPiGPIOPin physical_pin)
{
    bcm2835_gpio_clr(physical_pin);
}

void BcmManager::main_loop()
{
    while (_kill_flag.get_kill()) {
        
    }
}

void* bcm_manager_main_loop(void* obj)
{
    (BcmManager*) bcm_mgr_ptr = (BcmManager*) obj;
    bcm_mgr_ptr->main_loop(); 
}
