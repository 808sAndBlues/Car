#ifndef KILLFLAG_H
#define KILLFLAG_H

#include <atomic>
#include <pthread.h>
#include <iostream>

class KillFlag
{
    private:
        // TODO: Store this into a Mutex class
        pthread_mutex_t _mutex;

        std::atomic<bool> _kill_flag{false};

    public:
        void kill();

        std::atomic<bool>& get_kill_flag_ref();
};


#endif
