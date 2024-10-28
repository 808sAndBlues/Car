#include "Client.h"


void Client::main_loop()
{
    int sent = sendto(_socket_fd, "Hello", sizeof("Hello"), 0,
                      (struct sockaddr*) &_addr, sizeof(_addr));
    if (sent == -1) {
        std::perror("sendto");
        _kill_flag.kill();
        std::exit(-1);
    }

    std::cout << "Sent: " << sent << "\n";

    while (!_kill_flag.get_kill()) {
        usleep(1000000);            
    }

    close(_socket_fd);
}


void Client::init()
{
    _socket_fd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);

    if (_socket_fd == -1) {
        _logger.log_debug("Client: Failed to create socket");
        std::perror("socket");
        std::exit(-1);
    }

    _addr.sin_family = AF_INET;
    _addr.sin_port = htons(PORT);
    _addr.sin_addr.s_addr = inet_addr(HOST);

    if (connect(_socket_fd,
                (struct sockaddr*) &_addr, sizeof(_addr)) == -1) {
        _logger.log_debug("Client: Failed to connect to address");
        std::perror("connect");
        std::exit(-1);
    }

    _logger.log_debug("Client: Connected to server");
}


void* client_main_loop(void* obj)
{
    Client* client = (Client*) obj;
    client->main_loop();

    return nullptr;
}
