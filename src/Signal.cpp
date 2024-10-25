#include "Signal.h"

void Signal::init()
{
    _epoll.init();

    set_signal_masks();
    set_signal_fds();
    set_timer_fd();
    
    _logger.log_debug("Signal: Ready to go!");
}

void Signal::set_timer_fd()
{
    _timer_fd = timerfd_create(CLOCK_BOOTTIME, 0);
    if (_timer_fd == -1) {
        std::cout << "Failed to create timerfd\n";
        std::perror("timerfd_create");
        std::exit(-1);
    }
    
    struct itimerspec timer_spec = {0};
    timer_spec.it_interval.tv_sec = 1;
    timer_spec.it_interval.tv_nsec = 0;
    timer_spec.it_value.tv_sec = 1;
    timer_spec.it_value.tv_nsec = 0;

    if (timerfd_settime(_timer_fd, 0, &timer_spec, nullptr) == -1) {
        std::cout << "Failed to set timer\n";
        std::perror("timerfd_settime");
        std::exit(-1);
    }
    
    
    // TODO: Replace below w/ Epoll stuff

    if (_epoll.add_fd(_timer_fd) == -1) {
        _logger.log_debug("Signal: Failed to add timer fd");
        std::perror("Epoll::add_fd");
        std::exit(-1);
    }
}

void Signal::set_signal_fds()
{
    // Create signalfd and register to epoll
    if ((_signal_fd = signalfd(-1, &_signal_set, 0)) == -1) {
        std::cout << "Failed to create a signalfd\n";
        std::perror("signalfd");
        std::exit(-1);
    }

    if (_epoll.add_fd(_signal_fd) == -1) {
        _logger.log_debug("Signal: Failed to add signal fd");
        std::perror("Epoll::add_fd");
        std::exit(-1);
    }
}

void Signal::set_signal_masks()
{
    if (sigemptyset(&_signal_set) == -1) {
        std::cout << "Error filling w/ empty set\n";
        std::perror("sigemptyset");
        std::exit(-1);
    }

    if (sigaddset(&_signal_set, SIGTERM) == -1) {
        std::cout << "Failed to add SIGTERM to signal set\n";
        std::perror("sigaddset");
        std::exit(-1);
    }

    if (sigaddset(&_signal_set, SIGINT) == -1) {
        std::cout << "Failed to add SIGINT to signal set\n";
        std::perror("sigaddset");
        std::exit(-1);
    }

    if (sigprocmask(SIG_BLOCK, &_signal_set, 0) == -1) {
        std::cout << "Failed to block signal set\n";
        std::perror("sigprocmask");
        std::exit(-1);
    }
}

void Signal::main_loop()
{
    /*
     * I want the Signal class to handle signals generated by myself or
     * any other thread such as the potential CommHandler
     * 
     * The Signal class will call the kill flag and upon seeing this,
     * the VehicleManager will then execute a "cleanup" sequence

     * Is this the best way to do it? Probably not, but it's okay
     */

    while (!_kill_flag.get_kill()) {
        poll_events();
    }
}

void Signal::poll_events()
{
    int num_fds = _epoll.poll_events(0);

    if (num_fds == -1) {
        std::cout << "Failed to wait\n";
        std::perror("epoll_wait");
        std::exit(-1);
    }

    evaluate_epoll_events(num_fds);
}

void Signal::evaluate_epoll_events(int fds)
{
    struct epoll_event* events = _epoll.get_events();
    int idx = 0;

    while (idx < fds) {
        if (events[idx].data.fd == _signal_fd) {
            process_signal_fd();  
        }

        else if (events[idx].data.fd == _timer_fd) {
            process_timer_fd();
        }

        else {
            _logger.log_debug("Unknown file descriptor");
        }

        ++idx;
    }
}

void Signal::process_timer_fd()
{
    // TODO: Read from timer fd

    std::uint64_t num_expirations = 0;
    int read_count = read(_timer_fd, &num_expirations, sizeof(num_expirations));

    if (read_count != sizeof(num_expirations)) {
        _logger.log_debug("Signal: Error reading timer fd");
    }

    else {
        _logger.log_debug("Signal: Timer expired!");
    }
}

void Signal::process_signal_fd()
{
    struct signalfd_siginfo sig_info = {0};
    int read_count = read(_signal_fd, &sig_info, sizeof(sig_info));      

    if (read_count != sizeof(sig_info)) {
        std::cout << "Read count not expected!\n";
    }

    switch (sig_info.ssi_signo) {
        case SIGINT:
            _logger.log_debug("Signal: Received SIGINT");
            shutdown_sequence();
            break;

        case SIGTERM:
            _logger.log_debug("Signal: Received SIGINTERM");
            shutdown_sequence();
            break;

        default:
            std::cout << "Uknown signal flag received " << sig_info.ssi_signo
                      << "\n";
            break;
    }
}

void Signal::shutdown_sequence()
{
    _logger.flush_log();
    _kill_flag.kill();
}

void* signal_main_loop(void* obj)
{
    Signal* sig = (Signal*) obj;
    sig->main_loop();

    return nullptr;
}

