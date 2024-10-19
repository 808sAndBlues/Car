#include "Signal.h"

void Signal::init()
{
    set_signal_masks();
    set_signal_fds();
}

void Signal::set_signal_fds()
{
    // Create epoll instance
    if ((_epoll_fd = epoll_create(MAX_EPOLL_EVENTS)) == -1) {
        std::cout << "Failed to create epoll instance\n";
        std::perror("epoll_create");
        std::exit(-1);
    }

    
    // Create signalfd and register to epoll
    if ((_signal_fd = signalfd(-1, &_signal_set, 0)) == -1) {
        std::cout << "Failed to create a signalfd\n";
        std::perror("signalfd");
        std::exit(-1);
    }
    
    struct epoll_event events[MAX_EPOLL_EVENTS];

    struct epoll_event ev = {0};
    ev.events = EPOLLIN;
    ev.data.fd = _signal_fd;

    if (epoll_ctl(_epoll_fd, EPOLL_CTL_ADD, _signal_fd, events) == -1) {
        std::cout << "Failed to register signalfd to epoll\n";
        std::perror("signalfd");
        std::exit(-1);
    }

    std::cout << "Ready to go!\n";
    _kill_flag.kill();
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

    // TODO: Update this w/ Log statements
    std::cout << "Ready to catch SIGTERM and SIGINT\n";
}

void Signal::main_loop()
{
    std::cout << "Signal is ready!\n";
}

void* signal_main_loop(void* obj)
{
    Signal* sig = (Signal*) obj;
    sig->main_loop();

    return nullptr;
}

