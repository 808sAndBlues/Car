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
            // TODO: Add logging statement for number of bytes sent
        }
    }
}

void Client::recv_data(void* buffer, size_t length, struct sockaddr* address)
{
    errno = 0;

    unsigned int size = sizeof(address);
    int received = recvfrom(_socket_fd, buffer, length, 0, address,
                            &size);

    if (received == -1 && errno != EAGAIN) {
        std::cout << "Errno: " << errno << "\n";
        _logger.log_debug("Client: Receive failure");
        std::perror("recvfrom");
        std::exit(-1);
    }

    else if (errno == EAGAIN) {
        _logger.log_debug("Client: Did not receive data from CarServer");
    }

    else {
        std::cout << "Client: Receive count = " << received << "\n";
    }
}

void Client::init()
{
    _socket_fd = socket(AF_INET, SOCK_DGRAM | SOCK_NONBLOCK, IPPROTO_UDP);

    if (_socket_fd == -1) {
        _logger.log_debug("Client: Failed to create socket");
        std::perror("socket");
        std::exit(-1);
    }

    struct timeval timeout_value = {0};
    timeout_value.tv_sec = TIMEOUT_VAL;
    timeout_value.tv_usec = 0;


    if (setsockopt(_socket_fd, SOL_SOCKET, SO_RCVTIMEO, &timeout_value,
                   sizeof(timeout_value)) == -1) {
        _logger.log_debug("Client: Failed to set receive timeout option");
        std::perror("setsockopt");
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
    // TODO: Check return value
    ::close(_socket_fd); 
}

