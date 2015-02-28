#pragma once

#include <sys/socket.h>

namespace mock_network {
namespace test {

int accept(int sockfd, struct sockaddr* addr, socklen_t* addrlen);
int bind(int sockfd, const struct sockaddr* addr, socklen_t addrlen);
int close(int fd);
int connect(int sockfd, const struct sockaddr* addr, socklen_t addrlen);
int listen(int sockfd, int backlog);
int setsockopt(int sockfd, int level, int optname, const void* optval,
               socklen_t optlen);
int socket(int domain, int type, int protocol);

}  // namespace test
}  // namespace mock_network
