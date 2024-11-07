#ifndef CLIENT_H
#define CLIENT_H

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <errno.h>

#include "Logger.h"
#include "Epoll.h"
#include "KillFlag.h"

#define PORT        65000
#define HOST        "10.0.0.5"

#define TIMEOUT_VAL 0.1f

class Client
{
    protected:
        //TODO: Add SocketStatus Enum value
        Logger& _logger;
        Epoll _epoll;

        int _socket_fd = 0;
        struct sockaddr_in _addr = {0};

    public:
        Client(Logger& logger) : _logger(logger) {}

        void init();

        void send_data(std::uint8_t *buf, int length);

        void close();

        void recv_data(void* buffer, size_t length, struct sockaddr* addres);
};

#endif
