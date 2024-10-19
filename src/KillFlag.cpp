#include "KillFlag.h"

void KillFlag::kill()
{
    //TODO: Keep this behind a mutex
    if(pthread_mutex_lock(&_mutex) == -1) {
        std::cout << "Error locking mutex\n";
        std::perror("pthread_mutex_lock");
        std::exit(-1);
    }

    // TODO: Is this all I need to kill a thread?
    _kill_flag.store(true);

    if(pthread_mutex_unlock(&_mutex) == -1) {
        std::cout << "Error unlocking mutex\n";
        std::perror("pthread_mutex_unlock");
        std::exit(-1);
    }
}

std::atomic<bool>& KillFlag::get_kill()
{
    return _kill_flag;
}

