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

constexpr char* LOG_FILE = "car.log";

class Logger
{
    private:
        KillFlag& _kill_flag;
        Epoll     _epoll;

        bool _debug = true;

        int _timer_fd = 0;

        Queue _queue;


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

        void log_debug(char * msg);
        void log_error(char *msg);
        void log_warning(char *msg);

        void flush_log();


        void main_loop();
        void init();
};

void* logger_main(void* obj);


#endif
