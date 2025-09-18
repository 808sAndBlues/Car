#ifndef EPOLL_H
#define EPOLL_H

#include <sys/epoll.h>
#include <unistd.h>
#include <iostream>

#define MAX_EPOLL_EVENTS 255

class Epoll 
{
    private:

        int _epoll_fd = 0;

        struct epoll_event _events[MAX_EPOLL_EVENTS];

    public:
        Epoll() {};

        ~Epoll();

        // TODO: Change return value to enum status code
        int init();

        int add_fd(int fd);

        int poll_events(int ms_timeout);

        struct epoll_event* get_events();
};

#endif

