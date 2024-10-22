#ifndef SIGNAL_H
#define SIGNAL_H

#include <signal.h>
#include <sys/epoll.h>
#include <sys/signalfd.h>

#include "Logger.h"

constexpr int MAX_EPOLL_EVENTS = 255;

class Signal
{
    private:
        Logger& _logger;
        KillFlag& _kill_flag;
        
        int _epoll_fd = 0;
        int _signal_fd = 0;

        sigset_t _signal_set;


        void set_signal_masks();
        void set_signal_handler();
        void set_signal_fds();

        void poll_events();

        void process_signal_fd();

        void evaluate_epoll_events(int num_fds, struct epoll_event* events);

    public:
        Signal(Logger& logger, KillFlag& kill_flag) : _logger(logger),
                                                      _kill_flag(kill_flag) {}

        void init();
        void main_loop();
         
};

void* signal_main_loop(void* obj);


#endif
