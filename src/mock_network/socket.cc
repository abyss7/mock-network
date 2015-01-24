#include <mock_network/socket.h>

#include <base/aliases.h>

#include <unistd.h>

namespace mock_network {

namespace {

Mutex sockets_mutex;
HashMap<int, Socket> sockets;

}  // namespace

Socket::Socket(int domain, int type, int protocol)
    : domain_(domain), type_(type), protocol_(protocol) {
  if (pipe(pipe_) == -1) {
    return;
  }

  is_valid_ = true;

  UniqueLock lock(sockets_mutex);

  sockets.emplace(pipe_[0], *this);
  sockets.emplace(pipe_[1], *this);
}

// static
Socket Socket::Get(int fd) {
  UniqueLock lock(sockets_mutex);

  auto it = sockets.find(fd);
  if (it != sockets.end() && it->second.pipe_[0] == fd) {
    return it->second;
  }

  return Socket();
}

bool Socket::SetState(State state) {
  if (state_ == IDLE && state == CONNECTING) {
    state_ = state;
    return true;
  } else if (state_ == CONNECTING && state == CONNECTED) {
    state_ = state;
    return true;
  }

  return false;
}

int Socket::Close() {
  if (!is_valid_) {
    errno = EBADF;
    return -1;
  }

  UniqueLock lock(sockets_mutex);

  sockets.erase(pipe_[0]);
  sockets.erase(pipe_[1]);

  return ::close(pipe_[0]) | ::close(pipe_[1]);
}

}  // namespace mock_network
