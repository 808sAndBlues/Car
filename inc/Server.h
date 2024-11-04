#ifndef SERVER_H
#define SERVER_H

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <errno.h>

#include "Logger.h"

#define SERVER_HOST "0.0.0.0"
#define SERVER_PORT 65001

class Server
{
    protected:
        Logger& _logger;

        int _sock_fd = 0;

    public:
        Server(Logger& logger): _logger(logger) {}

        void init();

        ~Server();

        void recv_data(void* buffer, size_t length, struct sockaddr* addres);
};


#endif
