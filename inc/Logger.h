#ifndef LOGGER_H
#define LOGGER_H

// TODO: Fix multiple warning on include

#include <pthread.h>
#include <iostream>
#include <unistd.h>
#include <sys/timerfd.h>
#include <time.h>

#include "KillFlag.h"
#include "Queue.h"
#include "Epoll.h"

constexpr const char* LOG_FILE = "car.log";

class Logger
{
    private:
        KillFlag& _kill_flag;
        Epoll     _epoll;
        Queue _queue;

        bool _debug         = true;

        int _timer_fd       = 0;

        void setup_epoll();

        void setup_timerfd();

        void poll_events();

        void evaluate_events(int fds);

        void handle_expired_timer();
        
    public:
        Logger(KillFlag &kill_flag, bool debug) : _kill_flag(kill_flag),
                                                  _queue(LOG_FILE),
                                                  _debug(debug)
        {
        }

        void log_debug(const char * msg);
        void log_error(const char *msg);
        void log_warning(const char *msg);

        void flush_log();


        void main_loop();
        void init();
};

void* logger_main(void* obj);


#endif
