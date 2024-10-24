#ifndef LOGGER_H
#define LOGGER_H

// TODO: Fix multiple warning on include

#include <pthread.h>
#include <iostream>

#include "KillFlag.h"
#include "Queue.h"

constexpr char* LOG_FILE = "car.log";

class Logger
{
    private:
        KillFlag& _kill_flag;
        bool _debug = true;

        Queue _queue; 
        
    public:
        Logger(KillFlag &kill_flag, bool debug) : _kill_flag(kill_flag),
                                                  _queue(LOG_FILE)
        {
            _debug = debug;
        }

        void log_debug(char * msg);
        void log_error(char *msg);
        void log_warning(char *msg);

        void main_loop();
};

void* logger_main(void* obj);


#endif
