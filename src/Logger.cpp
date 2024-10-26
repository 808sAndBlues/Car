#include "Logger.h"

void Logger::main_loop()
{
    while (!_kill_flag.get_kill()) {
        poll_events();
    }
}

void Logger::init()
{
    setup_epoll();
    setup_timerfd();

    log_debug("Logger: Ready to go!");
}

void Logger::setup_epoll()
{
    if (_epoll.init() == -1) {
        log_debug("Logger: Error creating epoll object");
        std::perror("Epoll::init");
        std::exit(-1);
    }
}

void Logger::setup_timerfd()
{
    _timer_fd = timerfd_create(CLOCK_BOOTTIME, 0);

    if (_timer_fd == -1) {
        log_debug("Logger: Error creating timerfd");
        std::perror("timerfd_create");
        std::exit(-1);
    }

    struct itimerspec timer_spec = {0};
    timer_spec.it_interval.tv_sec = 1;
    timer_spec.it_interval.tv_nsec = 0;
    timer_spec.it_value.tv_sec = 1;
    timer_spec.it_value.tv_nsec = 0;

    if (timerfd_settime(_timer_fd, 0, &timer_spec, nullptr) == -1) {
        log_debug("Logger: Error setting timer values");
        std::perror("timerfd_settime");
        std::exit(-1);
    }

    if (_epoll.add_fd(_timer_fd) == -1) {
        log_debug("Logger: Error adding timerfd to Epoll object");
        std::perror("Epoll::add_fd");
        std::exit(-1);
    }

    log_debug("Logger: Added timer to Epoll object");
}

void Logger::poll_events()
{
    int num_fds = _epoll.poll_events(0);
    
    if (num_fds == -1) {
        log_debug("Logger: Failed to poll events");
        std::perror("Epoll::poll_events");
        std::exit(-1);
    }

    evaluate_events(num_fds);
}

void Logger::handle_expired_timer()
{
    log_debug("Logger: Timer expired!");
    
    std::uint64_t exp = 0;
    int read_count = read(_timer_fd, &exp, sizeof(exp));

    if (read_count != sizeof(exp)) {
        log_debug("Logger: Mismatching read from timer fd");
    }

    flush_log();
}

void Logger::evaluate_events(int fds)
{
    if (fds >= 0) {
        struct epoll_event* events = _epoll.get_events();

        for (int i = 0; i < fds; ++i) {
            if (events[i].data.fd == _timer_fd) {
                handle_expired_timer(); 
            }
        }
    }

    else {
        log_debug("Logger: Cannot evaluate negative amount of fds");
    }
}


void Logger::log_debug(const char* msg)
{
    _queue.append(msg);
}

void Logger::flush_log()
{
    _queue.flush();
}

void* logger_main(void *obj)
{
    Logger* logger = (Logger*) obj;
    logger->main_loop();

    return nullptr;
}


