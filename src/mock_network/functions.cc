#include <mock_network/functions.h>

#include <mock_network/socket.h>

#include <thread>

#include <errno.h>
#include <unistd.h>

namespace mock_network {

extern std::chrono::seconds connection_duration;
extern int connection_result;
extern Map<String, void*> originals;

namespace fake {

int close(int fd) {
  Socket socket = Socket::Get(fd);
  if (socket.IsValid()) {
    return socket.Close();
  }

  return reinterpret_cast<int (*)(int)>(originals["close"])(fd);
}

int connect(int sockfd, const struct sockaddr* addr, socklen_t addrlen) {
  Socket socket = Socket::Get(sockfd);
  if (!socket.IsValid()) {
    // We can't detect, if the |sockfd| is a valid descriptor at a system level,
    // so we always indicate that it's just not a socket.
    errno = ENOTSOCK;
    return -1;
  }

  if (!socket.SetState(Socket::CONNECTING)) {
    if (socket.GetState() == Socket::CONNECTING) {
      errno = EALREADY;
    } else if (socket.GetState() >= Socket::CONNECTED) {
      errno = EISCONN;
    }

    return -1;
  }

  if (!socket.IsNonBlocking()) {
    std::this_thread::sleep_for(connection_duration);
    // TODO: handle peer address.
    socket.SetState(Socket::CONNECTED);
  } else {
    // TODO: implement non-blocking connection, keeping in mind, that |select()|
    //       or |epoll()| should somehow indicate this fact using pipes.
    // TODO: return EINPROGRESS, if the |connection_duration| is non-zero.
  }

  if (connection_result == 0) {
    return 0;
  } else {
    errno = connection_result;
    return -1;
  }
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
