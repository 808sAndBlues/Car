#include "Epoll.h"

Epoll::Epoll()
{
    // TODO: Fill if necessary  
}

int Epoll::init()
{
    _epoll_fd = epoll_create(MAX_EPOLL_EVENTS);
    return _epoll_fd;
}

int Epoll::add_fd(int fd)
{
    struct epoll_event ev = {0};
    ev.data.fd = fd;
    ev.events = EPOLLIN;

    return epoll_ctl(_epoll_fd, EPOLL_CTL_ADD, fd, &ev);
}

int Epoll::poll_events(int ms_timeout)
{
    return epoll_wait(_epoll_fd, _events, MAX_EPOLL_EVENTS, ms_timeout);
}

struct epoll_event* Epoll::get_events()
{
    return _events;
}

Epoll::~Epoll()
{
    close(_epoll_fd);
}
