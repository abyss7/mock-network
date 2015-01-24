#include <mock_network/test_library.h>

#include <unistd.h>

namespace mock_network {
namespace test {

int close(int fd) {
  return ::close(fd);
}

int connect(int sockfd, const struct sockaddr* addr, socklen_t addrlen) {
  return ::connect(sockfd, addr, addrlen);
}

int socket(int domain, int type, int protocol) {
  return ::socket(domain, type, protocol);
}

}  // namespace test
}  // namespace mock_network
