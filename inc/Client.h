#ifndef CLIENT_H
#define CLIENT_H

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

#include "Logger.h"
#include "Epoll.h"
#include "KillFlag.h"

#define PORT    65000
#define HOST    "10.0.0.5"

class Client
{

    protected:
        //TODO: Add SocketStatus Enum value
        Logger& _logger;
        KillFlag& _kill_flag;
        Epoll _epoll;

        int _socket_fd = 0;
        struct sockaddr_in _addr = {0};

    public:
        Client(Logger& logger, KillFlag& kill_flag) : _logger(logger),
                                                      _kill_flag(kill_flag) {}

        void init();

        void main_loop();

};

void* client_main_loop(void* obj);



#endif
