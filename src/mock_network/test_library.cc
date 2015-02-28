#include <mock_network/test_library.h>

#include <unistd.h>

namespace mock_network {
namespace test {

int accept(int sockfd, struct sockaddr* addr, socklen_t* addrlen) {
  return ::accept(sockfd, addr, addrlen);
}

int bind(int sockfd, const struct sockaddr* addr, socklen_t addrlen) {
  return ::bind(sockfd, addr, addrlen);
}

int close(int fd) {
  return ::close(fd);
}

int connect(int sockfd, const struct sockaddr* addr, socklen_t addrlen) {
  return ::connect(sockfd, addr, addrlen);
}

int listen(int sockfd, int backlog) {
  return ::listen(sockfd, backlog);
}

int setsockopt(int sockfd, int level, int optname, const void* optval,
               socklen_t optlen) {
  return ::setsockopt(sockfd, level, optname, optval, optlen);
}

int socket(int domain, int type, int protocol) {
  return ::socket(domain, type, protocol);
}

}  // namespace test
}  // namespace mock_network
