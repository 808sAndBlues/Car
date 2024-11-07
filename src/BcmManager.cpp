#include "BcmManager.h"

BcmManager::BcmManager(Logger& logger,
                       KillFlag& kill_flag) : _logger(logger),
                                              _kill_flag(kill_flag),
                                              _client(logger),
                                              _server(logger)
{
    _client.init();
    _server.init();
}

void BcmManager::setup_epoll()
{
    _epoll.init();
    setup_tlm_timerfd();
    setup_control_timerfd();
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

    setup_pwm();

    setup_motors();

    _logger.log_debug("BcmManager: Initialized");
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

    _logger.flush_log();
}

void BcmManager::setup_tlm_timerfd()
{
    _tlm_timerfd = timerfd_create(CLOCK_BOOTTIME, 0);

    struct itimerspec timer_spec = {0};
    timer_spec.it_value.tv_sec = 1;
    timer_spec.it_value.tv_nsec = 0;
    timer_spec.it_interval.tv_sec = 1;
    timer_spec.it_interval.tv_nsec = 0;

    if (timerfd_settime(_tlm_timerfd, 0, &timer_spec, nullptr) == -1) {
        _logger.log_debug("BcmManager: Failed to setup tlm timerfd");
        std::perror("timerfd_settime");
        std::exit(-1);
    }

    if (_epoll.add_fd(_tlm_timerfd) == -1) {
        _logger.log_debug("BcmManager: Failed to add tlm timerfd");
        std::perror("epoll_ctl");
        std::exit(-1);
    }
}

void BcmManager::setup_control_timerfd()
{
    _control_timerfd = timerfd_create(CLOCK_BOOTTIME, 0);

    struct itimerspec timer_spec = {0};
    timer_spec.it_value.tv_sec = 1;
    timer_spec.it_value.tv_nsec = 0;
    timer_spec.it_interval.tv_sec = 0;
    timer_spec.it_interval.tv_nsec = CONTROL_POLL_NS;

    if (timerfd_settime(_control_timerfd, 0, &timer_spec, nullptr) == -1) {
        _logger.log_debug("BcmManager: Failed to setup control timerfd");
        std::perror("timerfd_settime");
        std::exit(-1);
    }

    if (_epoll.add_fd(_control_timerfd) == -1) {
        _logger.log_debug("BcmManager: Failed to add tlm timerfd");
        std::perror("epoll_ctl");
        std::exit(-1);
    }
}

void BcmManager::send_telemetry()
{
    send_gpio_status();
    send_time_status();
    send_motor_status();
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
        if (events[i].data.fd == _tlm_timerfd) {
            tlm_timer_handler();
        }

        else if (events[i].data.fd == _control_timerfd) {
            control_timer_handler();
        }
    }
}

void BcmManager::control_timer_handler()
{
    std::uint64_t num_expirations = 0;
    int count = read(_control_timerfd,
                     &num_expirations, sizeof(std::uint64_t));
    
    if (count == -1) {
        _logger.log_debug("BcmManager: Failed to read from timerfd");
        std::perror("read");
        std::exit(-1);
    }
    else {
        std::memset(_recv_buffer, 0, sizeof(_recv_buffer));

        struct sockaddr_in cli_addr = {0};
        int received = _server.recv_data(_recv_buffer, sizeof(_recv_buffer),
                          (struct sockaddr*) &cli_addr);

        process_control_data(received);
    }
}

void BcmManager::tlm_timer_handler()
{
    std::uint64_t num_expirations = 0;
    int count = read(_tlm_timerfd, &num_expirations, sizeof(std::uint64_t));
    
    if (count == -1) {
        _logger.log_debug("BcmManager: Failed to read from timerfd");
        std::perror("read");
        std::exit(-1);
    }

    else {
        send_telemetry();
    }
}

void BcmManager::send_motor_status()
{
    std::uint8_t buf[sizeof(MotorStatus)] = {0};

    update_motor_status();
    
    serialize_motor_status(buf, sizeof(MotorStatus));

    _client.send_data(buf, sizeof(MotorStatus));
}

//TODO: Remove size paramater from serializing functions
void BcmManager::serialize_motor_status(std::uint8_t* buf, size_t size)
{
    std::uint8_t* ptr = buf;
    
    *ptr = _motor_status.header;
    ++ptr;

    *ptr = _motor_status.len;
    ++ptr;

    *ptr = _motor_status.id;
    ++ptr;

    *ptr =  static_cast<std::uint8_t>(_motor_status.motor_a_on);
    ++ptr;

    *ptr = _motor_status.motor_a_state;
    ++ptr;

    *ptr = static_cast<std::uint8_t>(_motor_status.motor_b_on);
    ++ptr;

    *ptr = _motor_status.motor_b_state;
    ++ptr;

    *ptr = _motor_status.tlr;
    ++ptr;
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

std::uint8_t BcmManager::get_level(RPiGPIOPin pin)
{
    return bcm2835_gpio_lev(pin);
}

void BcmManager::update_motor_a_state()
{
    _motor_status.motor_a_on = get_level(MOTOR_A_PWM_PIN);

    if (_motor_status.motor_a_on) {
        if (get_level(MOTOR_A_FWD_PIN)) {
            _motor_status.motor_a_state = FORWARD;
        }

        else if (get_level(MOTOR_A_REVERSE_PIN)) {
            _motor_status.motor_a_state = REVERSE;
        }

        else {
            _motor_status.motor_a_state = IDLE;
        }
    }

    else {
        _motor_status.motor_a_state = OFF;
    }
}

void BcmManager::update_motor_b_state()
{
    _motor_status.motor_b_on = get_level(MOTOR_B_PWM_PIN);

    if (_motor_status.motor_b_on) {
        if (get_level(MOTOR_B_FWD_PIN)) {
            _motor_status.motor_b_state = FORWARD;
        }

        else if (get_level(MOTOR_B_REVERSE_PIN)) {
            _motor_status.motor_b_state = REVERSE;
        }

        else {
            _motor_status.motor_b_state = IDLE;
        }
    }

    else {
        _motor_status.motor_b_state = IDLE; 
    }
}

void BcmManager::update_motor_status()
{
    update_motor_a_state();
    update_motor_b_state();
}

void BcmManager::setup_motor_a()
{
    bcm2835_gpio_fsel(MOTOR_A_FWD_PIN, BCM2835_GPIO_FSEL_OUTP);
    clear_pin(MOTOR_A_FWD_PIN);

    bcm2835_gpio_fsel(MOTOR_A_REVERSE_PIN, BCM2835_GPIO_FSEL_OUTP);
    clear_pin(MOTOR_A_REVERSE_PIN);
}

void BcmManager::setup_motor_b()
{
    bcm2835_gpio_fsel(MOTOR_B_FWD_PIN, BCM2835_GPIO_FSEL_OUTP);
    clear_pin(MOTOR_B_FWD_PIN);

    bcm2835_gpio_fsel(MOTOR_B_REVERSE_PIN, BCM2835_GPIO_FSEL_OUTP);
    clear_pin(MOTOR_B_REVERSE_PIN);
}

void BcmManager::motor_a_forward()
{
    clear_pin(MOTOR_A_REVERSE_PIN);
    set_pin(MOTOR_A_FWD_PIN);
}

void BcmManager::motor_b_forward()
{
    clear_pin(MOTOR_B_REVERSE_PIN);
    set_pin(MOTOR_B_FWD_PIN);
}

void BcmManager::motor_a_backward()
{
    clear_pin(MOTOR_A_FWD_PIN);
    set_pin(MOTOR_A_REVERSE_PIN);

    // TODO: Add logging statement
}

void BcmManager::motor_b_backward()
{
    clear_pin(MOTOR_B_FWD_PIN);
    set_pin(MOTOR_B_REVERSE_PIN);

    // TODO: Add logging statement
}

void BcmManager::setup_pwm_pins()
{
    bcm2835_gpio_fsel(MOTOR_A_PWM_PIN, BCM2835_GPIO_FSEL_ALT5);
    bcm2835_gpio_fsel(MOTOR_B_PWM_PIN, BCM2835_GPIO_FSEL_ALT0);
}

void BcmManager::setup_pwm()
{
    setup_pwm_pins();

    bcm2835_pwm_set_clock(CLOCK_DIV);
    bcm2835_pwm_set_mode(PWM_CHANNEL_0, 1, 1);
    bcm2835_pwm_set_range(PWM_CHANNEL_0, WRAP);
}

void BcmManager::drive_forward(std::uint16_t data)
{
    motor_a_forward();
    motor_b_forward();

    bcm2835_pwm_set_data(PWM_CHANNEL_0, data);
}

void BcmManager::drive_backward(std::uint16_t data)
{
    motor_a_backward();
    motor_b_backward();

    bcm2835_pwm_set_data(PWM_CHANNEL_0, data);
}

void BcmManager::process_control_data(int received)
{
    _received_ctl_val = DFLT_CTL_VAL;

    if (received > 0) {
        std::memcpy(&_received_ctl_val, _recv_buffer, received);

        switch (_received_ctl_val) {
            case FORWARD_CTL_VAL:
                // TODO: Add logging statement
                drive_forward(WRAP * DUTY_RATIO);
                break;

            case REVERSE_CTL_VAL:
                // TODO: Add logging statement
                drive_backward(WRAP * DUTY_RATIO);
                break;
            default:
                shutdown_pwm();
                break;
        }
    }

    else {
        shutdown_pwm();
    }
}

void BcmManager::main_loop()
{
    set_adventure_led();

    while (!_kill_flag.get_kill()) {
        poll_events();
    }
}

void BcmManager::shutdown_sequence()
{
    shutdown_pwm();
    close_adventure_led();
    send_telemetry();
    _client.close();

    _logger.log_debug("BcmManager: Shutdown complete!");
}

void BcmManager::shutdown_pwm()
{
    bcm2835_pwm_set_data(PWM_CHANNEL_0, 0);

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

