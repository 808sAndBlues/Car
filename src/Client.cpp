#include "Client.h"

void Client::send_data(std::uint8_t* buf, int length)
{
    std::uint32_t sent = 0;
    std::uint32_t remaining = length;

    while (remaining > 0) {
        sent = sendto(_socket_fd, buf + (length - remaining), 
                      remaining, 0, (struct sockaddr*) &_addr, sizeof(_addr));

        if (sent == -1) {
            _logger.log_debug("Client: Failed to send data");
            std::perror("sendto");
            std::exit(-1);
        }

        else {
            remaining -= sent;
            
            // TODO: Add logging statement for number of 
        }
    }
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

void Client::close()
{
    ::close(_socket_fd); 
}

