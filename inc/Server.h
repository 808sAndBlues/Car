#ifndef SERVER_H
#define SERVER_H

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <errno.h>

#include "Logger.h"

#define SERVER_HOST  "0.0.0.0"
#define SERVER_PORT  65003

#define TIMEOUT_RECV 2

const unsigned int SIZEOF_ADDRESS = sizeof(struct sockaddr*);

class Server
{
    protected:
        Logger& _logger;

        int _sock_fd = 0;

        unsigned int _address_holder = SIZEOF_ADDRESS;

    public:
        Server(Logger& logger): _logger(logger) {}

        void init();

        ~Server();

        int recv_data(void* buffer, size_t length, struct sockaddr* addres);
};


#endif
