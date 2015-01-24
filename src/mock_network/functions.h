#pragma once

#include <sys/socket.h>

namespace mock_network {
namespace fake {

int close(int fd);
int connect(int sockfd, const struct sockaddr* addr, socklen_t addrlen);
int socket(int domain, int type, int protocol);

}  // namespace fake
}  // namespace mock_network
