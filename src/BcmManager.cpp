#include "BcmManager.h"

BcmManager::BcmManager(Logger& logger,
                       KillFlag& kill_flag) : _logger(logger),
                                              _kill_flag(kill_flag),
                                              _client(logger)
{
    _client.init();
}

void BcmManager::setup_epoll()
{
    _epoll.init();
    setup_timerfd();
}

void BcmManager::init()
{
    setup_epoll();

    if (!bcm2835_init()) {
        _logger.log_debug("Failed to init BCM2385 lbirary");
        std::perror("bcm2835.init()");
        std::exit(-1);
    }

    // Clear all pins
    for (std::uint8_t i = 0; i < GPIO_COUNT; ++i) {
        bcm2835_gpio_clr(GPIO_PINS[i]);
    }

    set_init_led();

    setup_pwm();
    setup_motors();



    _logger.log_debug("BcmManager: Initialized");
}

void BcmManager::set_init_led()
{
    //TODO: Set GPIO to output
    bcm2835_gpio_fsel(INIT_LED_GPIO_PIN, BCM2835_GPIO_FSEL_OUTP);
    set_pin(INIT_LED_GPIO_PIN);

    _logger.log_debug("BcmManager: Init LED has been turned on");
}

void BcmManager::close_init_led()
{
    clear_pin(INIT_LED_GPIO_PIN);
    _logger.log_debug("BcmManager: Init LED has been turned off");
}

void BcmManager::set_adventure_led()
{
    bcm2835_gpio_fsel(ADVENTURE_LED_GPIO_PIN, BCM2835_GPIO_FSEL_OUTP);
    set_pin(ADVENTURE_LED_GPIO_PIN);

    _logger.log_debug("BcmManager: Adventure LED has been lit");
}

void BcmManager::close_adventure_led()
{
    clear_pin(ADVENTURE_LED_GPIO_PIN);
    _logger.log_debug("BcmManager: Adventure LED has been turned off");

    // TODO: Force a flush since turning off the adventure LED indicates a 
    // problem/shutdown

    _logger.flush_log();
}

void BcmManager::setup_timerfd()
{
    _timerfd = timerfd_create(CLOCK_BOOTTIME, 0);

    struct itimerspec timer_spec = {0};
    timer_spec.it_value.tv_sec = 1;
    timer_spec.it_value.tv_nsec = 0;
    timer_spec.it_interval.tv_sec = 1;
    timer_spec.it_interval.tv_nsec = 0;

    if (timerfd_settime(_timerfd, 0, &timer_spec, nullptr) == -1) {
        _logger.log_debug("BcmManager: Failed to setup timerfd");
        std::perror("timerfd_settime");
        std::exit(-1);
    }

    if (_epoll.add_fd(_timerfd) == -1) {
        _logger.log_debug("BcmManager: Failed to add timerfd");
        std::perror("epoll_ctl");
        std::exit(-1);
    }
}

void BcmManager::send_telemetry()
{
    send_gpio_status();
    send_time_status();
}

void BcmManager::poll_events()
{ 
   int fds = _epoll.poll_events(0);
   evaluate_events(fds);
}

void BcmManager::evaluate_events(int fds)
{
    struct epoll_event* events = _epoll.get_events();

    for (int i = 0; i < fds; ++i) {
        if (events[i].data.fd == _timerfd) {
            // TODO: Call timer handler
            timer_handler();
        }
    }
}

void BcmManager::timer_handler()
{
    std::uint64_t num_expirations = 0;
    int count = read(_timerfd, &num_expirations, sizeof(std::uint64_t));
    
    if (count == -1) {
        _logger.log_debug("BcmManager: Failed to read from timerfd");
        std::perror("read");
        std::exit(-1);
    }

    else {
        // TODO: Update this to be a "general" telemetry send function
        send_gpio_status();
        send_time_status();
    }
}

void BcmManager::send_time_status()
{
    std::uint8_t buf[sizeof(TimeStatus)] = {0};

    update_time_status();

    serialize_time_status(buf, sizeof(TimeStatus));

    _client.send_data(buf, sizeof(TimeStatus));

    _logger.log_debug("BcmManager: Sent TimeStatus");
}

void BcmManager::serialize_time_status(std::uint8_t* buf, int length)
{
    std::uint8_t* ptr = buf;
    *ptr = _time_status.header;
    ++ptr;

    *ptr = _time_status.len;
    ++ptr;

    *ptr = _time_status.id;
    ++ptr;

    std::uint32_t be_elapsed_time = htobe32(_time_status.t_seconds);  

    std::memcpy(ptr, &be_elapsed_time, sizeof(be_elapsed_time));
    ptr += sizeof(be_elapsed_time);

    *ptr = _time_status.tlr;
}

void BcmManager::update_time_status()
{
    _time_status.header = TELEMETRY_HDR;
    _time_status.id = TIME_STATUS;
    _time_status.len = TIME_STATUS_LEN;
    _time_status.tlr = TELEMETRY_TLR;

    _time_status.t_seconds += 1;
    
}

void BcmManager::send_gpio_status()
{

    std::uint8_t buf[sizeof(GPIOStatus)] = {0};

    update_gpio_status();
    
    serialize_gpio_status(buf, sizeof(GPIOStatus));

    _client.send_data(buf, sizeof(GPIOStatus));

    _logger.log_debug("BcmManager: Sent GPIO Status");
}

void BcmManager::update_gpio_status()
{
    _gpio_status.header = TELEMETRY_HDR;
    _gpio_status.id = GPIO_STATUS;
    _gpio_status.len = GPIO_STATUS_LEN;
    _gpio_status.tlr = TELEMETRY_TLR;

    for (std::uint8_t i = 0; i < GPIO_COUNT; ++i) {
        _gpio_status.gpio_status[i] = bcm2835_gpio_lev(GPIO_PINS[i]);
    }
}

void BcmManager::serialize_gpio_status(std::uint8_t* buf, int length)
{
    std::uint8_t* ptr = buf;

    // Set header
    *ptr = _gpio_status.header;
    ++ptr;

    // Set len
    *ptr = _gpio_status.len;
    ++ptr; 

    // Set id
    *ptr = _gpio_status.id;
    ++ptr;

    for (std::uint8_t i = 0; i < GPIO_COUNT; ++i) {
        *ptr = _gpio_status.gpio_status[i];
        ++ptr; 
    }

    // Set tlr
    *ptr = _gpio_status.tlr;
}

BcmManager::~BcmManager()
{
    // TODO: Check return code
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

void BcmManager::setup_motors()
{
    setup_motor_a();
    setup_motor_b();
}

void BcmManager::setup_motor_a()
{
    bcm2835_gpio_fsel(RPI_BPLUS_GPIO_J8_16, BCM2835_GPIO_FSEL_OUTP);
    clear_pin(RPI_BPLUS_GPIO_J8_16);

    bcm2835_gpio_fsel(RPI_BPLUS_GPIO_J8_18, BCM2835_GPIO_FSEL_OUTP);
    clear_pin(RPI_BPLUS_GPIO_J8_18);
}

void BcmManager::setup_motor_b()
{
    bcm2835_gpio_fsel(RPI_BPLUS_GPIO_J8_26, BCM2835_GPIO_FSEL_OUTP);
    clear_pin(RPI_BPLUS_GPIO_J8_26);

    bcm2835_gpio_fsel(RPI_BPLUS_GPIO_J8_24, BCM2835_GPIO_FSEL_OUTP);
    clear_pin(RPI_BPLUS_GPIO_J8_24);
}

void BcmManager::motor_a_forward()
{
    set_pin(MOTOR_A_FWD_PIN);
}

void BcmManager::motor_b_forward()
{
    set_pin(MOTOR_B_FWD_PIN);
}

void BcmManager::setup_pwm_pins()
{
    bcm2835_gpio_fsel(RPI_BPLUS_GPIO_J8_12, BCM2835_GPIO_FSEL_ALT5);
    bcm2835_gpio_fsel(RPI_BPLUS_GPIO_J8_32, BCM2835_GPIO_FSEL_ALT0);
}

void BcmManager::setup_pwm()
{
    setup_pwm_pins();

    bcm2835_pwm_set_clock(CLOCK_DIV);
    bcm2835_pwm_set_mode(0, 1, 1);
    bcm2835_pwm_set_range(0, WRAP);
}

void BcmManager::drive_forward(std::uint16_t data)
{
    bcm2835_pwm_set_data(0, data);

    motor_a_forward();
    motor_b_forward();
}

void BcmManager::main_loop()
{
    close_init_led();
    set_adventure_led();

    drive_forward(WRAP/4);

    /*
    bcm2835_gpio_fsel(RPI_BPLUS_GPIO_J8_16, BCM2835_GPIO_FSEL_OUTP);
    clear_pin(RPI_BPLUS_GPIO_J8_16);

    bcm2835_gpio_fsel(RPI_BPLUS_GPIO_J8_18, BCM2835_GPIO_FSEL_OUTP);
    clear_pin(RPI_BPLUS_GPIO_J8_18);


    bcm2835_gpio_fsel(RPI_BPLUS_GPIO_J8_26, BCM2835_GPIO_FSEL_OUTP);
    clear_pin(RPI_BPLUS_GPIO_J8_26);

    bcm2835_gpio_fsel(RPI_BPLUS_GPIO_J8_24, BCM2835_GPIO_FSEL_OUTP);
    clear_pin(RPI_BPLUS_GPIO_J8_24);
    
    bcm2835_gpio_fsel(RPI_BPLUS_GPIO_J8_12, BCM2835_GPIO_FSEL_ALT5);
    bcm2835_gpio_fsel(RPI_BPLUS_GPIO_J8_32, BCM2835_GPIO_FSEL_ALT0);
    */

    //bcm2835_gpio_fsel(RPI_BPLUS_GPIO_J8_33, BCM2835_GPIO_FSEL_OUTP);
    /*bcm2835_pwm_set_clock(BCM2835_PWM_CLOCK_DIVIDER_16);
    bcm2835_pwm_set_mode(0, 1, 1);
    bcm2835_pwm_set_range(0, 1024);

    bcm2835_pwm_set_data(0, 1023);

    set_pin(RPI_BPLUS_GPIO_J8_16);
    set_pin(RPI_BPLUS_GPIO_J8_26);*/

    while (!_kill_flag.get_kill()) {
        poll_events();
    }

    shutdown_pwm();

    close_adventure_led();
    send_telemetry();
    _client.close();
}

void BcmManager::shutdown_pwm()
{
    bcm2835_pwm_set_data(0, 0);

    clear_pin(MOTOR_A_FWD_PIN);
    clear_pin(MOTOR_A_REVERSE_PIN);
    clear_pin(MOTOR_B_FWD_PIN);
    clear_pin(MOTOR_B_REVERSE_PIN);
}

void* bcm_manager_main_loop(void* obj)
{
    BcmManager* bcm_mgr_ptr = (BcmManager*) obj;
    bcm_mgr_ptr->main_loop();
    return nullptr;
}

void BcmManager::step(int val)
{
    switch(val)
    {
        case 0:
            set_pin(RPI_BPLUS_GPIO_J8_08);
            set_pin(RPI_BPLUS_GPIO_J8_10);
            clear_pin(RPI_BPLUS_GPIO_J8_12);
            clear_pin(RPI_BPLUS_GPIO_J8_16);
            break;
        case 1:
            clear_pin(RPI_BPLUS_GPIO_J8_08);
            set_pin(RPI_BPLUS_GPIO_J8_10);
            set_pin(RPI_BPLUS_GPIO_J8_12);
            clear_pin(RPI_BPLUS_GPIO_J8_16);
            break;

        case 2:
            clear_pin(RPI_BPLUS_GPIO_J8_08);
            clear_pin(RPI_BPLUS_GPIO_J8_10);
            set_pin(RPI_BPLUS_GPIO_J8_12);
            set_pin(RPI_BPLUS_GPIO_J8_16);
            break;

        case 3:
            set_pin(RPI_BPLUS_GPIO_J8_08);
            clear_pin(RPI_BPLUS_GPIO_J8_10);
            clear_pin(RPI_BPLUS_GPIO_J8_12);
            set_pin(RPI_BPLUS_GPIO_J8_16);
            break;

        default:
            clear_pin(RPI_BPLUS_GPIO_J8_08);
            clear_pin(RPI_BPLUS_GPIO_J8_10);
            clear_pin(RPI_BPLUS_GPIO_J8_12);
            clear_pin(RPI_BPLUS_GPIO_J8_16);

            break;
    }
}
