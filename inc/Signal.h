#ifndef SIGNAL_H
#define SIGNAL_H

#include <signal.h>
#include <sys/epoll.h>
#include <sys/signalfd.h>
#include <sys/timerfd.h>
#include <time.h>

#include "Logger.h"
#include "Epoll.h"

class Signal
{
    private:
        Logger& _logger;
        KillFlag& _kill_flag;
        Epoll _epoll;
        sigset_t _signal_set;
        
        int _signal_fd = 0;
        int _timer_fd = 0;

        void set_signal_masks();
        void set_signal_handler();
        void set_signal_fds();

        void set_timer_fd();

        void poll_events();

        void process_signal_fd();

        void process_timer_fd();

        void evaluate_epoll_events(int num_fds);

    public:
        Signal(Logger& logger, KillFlag& kill_flag) : _logger(logger),
                                                      _kill_flag(kill_flag) {}

        void init();
        void main_loop();

        void shutdown_sequence();
};

void* signal_main_loop(void* obj);

#endif
