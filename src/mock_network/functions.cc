#include <mock_network/functions.h>

#include <mock_network/connection.h>
#include <mock_network/mock.h>
#include <mock_network/socket.h>

#include <thread>

#include <errno.h>
#include <unistd.h>

namespace mock_network {
namespace fake {

int accept(int sockfd, struct sockaddr* addr, socklen_t* addrlen) {
  // TODO: implement this.
  return -1;
}

int bind(int sockfd, const struct sockaddr* addr, socklen_t addrlen) {
  Socket socket = Socket::Get(sockfd);
  if (!socket.IsValid()) {
    // We can't detect, if the |sockfd| is a valid descriptor at a system level,
    // so we always indicate that it's just not a socket.
    errno = ENOTSOCK;
    return -1;
  }

  if (!socket.SetState(Socket::BIND, addr, addrlen)) {
    // Assume |errno| is already set by |Socket::SetState()|.
    return -1;
  }

  return 0;
}

int close(int fd) {
  Socket socket = Socket::Get(fd);
  if (socket.IsValid()) {
    return socket.Close();
  }

  return Mock::CallOriginal<int>("close", fd);
}

int connect(int sockfd, const struct sockaddr* addr, socklen_t addrlen) {
  Socket socket = Socket::Get(sockfd);
  if (!socket.IsValid()) {
    // We can't detect, if the |sockfd| is a valid descriptor at a system level,
    // so we always indicate that it's just not a socket.
    errno = ENOTSOCK;
    return -1;
  }

  if (!socket.SetState(Socket::CONNECTING, addr, addrlen)) {
    // Assume |errno| is already set by |Socket::SetState()|.
    if (Connection::result_ != 0) {
      errno = Connection::result_;
    }
    return -1;
  }

  if (!socket.IsNonBlocking()) {
    std::this_thread::sleep_for(Connection::duration_);
    if (!socket.SetState(Socket::CONNECTED)) {
      errno = EINVAL;
      return -1;
    }
  } else {
    // TODO: implement non-blocking connection, keeping in mind, that |select()|
    //       or |epoll()| should somehow indicate this fact using pipes.
    // TODO: return EINPROGRESS, if the |connection_duration| is non-zero.
  }

  if (Connection::result_ == 0) {
    return 0;
  } else {
    errno = Connection::result_;
    return -1;
  }
}

int listen(int sockfd, int backlog) {
  Socket socket = Socket::Get(sockfd);
  if (!socket.IsValid()) {
    // We can't detect, if the |sockfd| is a valid descriptor at a system level,
    // so we always indicate that it's just not a socket.
    errno = ENOTSOCK;
    return -1;
  }

  if (!socket.SetState(Socket::PASSIVE)) {
    // Assume |errno| is already set by |Socket::SetState()|.
    return -1;
  }

  return 0;
}

int setsockopt(int sockfd, int level, int optname, const void* optval,
               socklen_t optlen) {
  // TODO: implement this.
  return 0;
}

int socket(int domain, int type, int protocol) {
  // We work only with predefined combinations of params.
  if ((domain != AF_UNIX && domain != AF_INET && domain != AF_INET6) ||
      (type & SOCK_STREAM) != SOCK_STREAM || protocol != 0) {
    errno = EINVAL;
    return -1;
  }

  return Socket(domain, type, protocol);
}

}  // namespace fake
}  // namespace mock_network
