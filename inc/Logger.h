#ifndef LOGGER_H
#define LOGGER_H

// TODO: Fix multiple warning on include

#include <pthread.h>
#include <iostream>
#include <atomic>

constexpr char* LOG_FILE = "car.log";

class Logger
{
    private:
        std::atomic<bool>& _kill_flag;
        


    public:
        Logger(const std::atomic<bool> &kill_flag) : _kill_flag{kill_flag} {}

        void log_debug(char * msg);
        void log_error(char *msg);
        void log_warning(char *msg);

        void main_loop(void* obj);

        
};

#endif
