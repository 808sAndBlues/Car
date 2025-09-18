#include "Server.h"

void Server::init()
{
    _sock_fd = socket(AF_INET, SOCK_DGRAM | SOCK_NONBLOCK, 0);

    if (_sock_fd == -1) {
        _logger.log_debug("Server: Failed to create server socket");
        std::perror("socket");
        std::exit(-1);
    }

    int flag = 1;

    if (setsockopt(_sock_fd, SOL_SOCKET, SO_REUSEADDR, &flag,
                   sizeof(flag)) == -1) {
        _logger.log_debug("Server: Failed to set socket option");
        std::perror("socket");
        std::exit(-1);
    }

    struct timeval timeout_value = {0};
    timeout_value.tv_sec = TIMEOUT_RECV;
    timeout_value.tv_usec = 0;

    if (setsockopt(_sock_fd, SOL_SOCKET, SO_RCVTIMEO, &timeout_value,
                   sizeof(timeout_value)) == -1) {
        _logger.log_debug("Server: Failed to set socket option");
        std::perror("socket");
        std::exit(-1);
    }

    struct sockaddr_in server_addr = {0};
    server_addr.sin_port = htons(SERVER_PORT);
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_family = AF_INET;

    if (bind(_sock_fd, (struct sockaddr*) &server_addr,
             sizeof(server_addr)) == -1) {
        _logger.log_debug("Server: Failed to bind server");
        std::perror("bind");
        std::exit(-1);
    }
}

int Server::recv_data(void* buffer, size_t length, struct sockaddr* address)
{
    errno = 0;

    int received = recvfrom(_sock_fd, buffer, length, 0, address,
                            &_address_holder);

    if (received == -1 && errno != EAGAIN) {
        _logger.log_debug("Server: Receive failure");
        std::perror("recvfrom");
        std::exit(-1);
    }

    else if (errno == EAGAIN) {
        _logger.log_debug("Server: Did not receive data from CarClient");
    }

    else {
        std::cout << "Server: Receive count = " << received << "\n";
    }

    return received;
}

Server::~Server()
{
    // TODO: Check return value
    ::close(_sock_fd);
}

