#ifndef SIGNAL_H
#define SIGNAL_H

#include <signal.h>


#include "Logger.h"

class Signal
{
    private:
        Logger& _logger;
        KillFlag& _kill_flag;

        void set_signal_masks();
        void set_signal_handler();

    public:
        Signal(Logger& logger, KillFlag& kill_flag) : _logger(logger),
                                                      _kill_flag(kill_flag) {}

        void init(); 
         
};

void signal_handler(int signum, siginfo_t* info, void* context); 

#endif
