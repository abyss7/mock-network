#include <mock_network/socket.h>

#include <base/aliases.h>
#include <mock_network/mock.h>

#include <string.h>
#include <unistd.h>

namespace mock_network {

namespace {

Mutex sockets_mutex;
HashMap<int, Socket> sockets;

}  // namespace

Socket::Socket(int domain, int type, int protocol) {
  if (pipe(details_->pipe) == -1) {
    return;
  }

  details_->domain = domain;
  details_->type = type;
  details_->protocol = protocol;
  details_->is_valid = true;

  UniqueLock lock(sockets_mutex);

  sockets.emplace(details_->pipe[0], *this);
  sockets.emplace(details_->pipe[1], *this);
}

// static
Socket Socket::Get(int fd) {
  UniqueLock lock(sockets_mutex);

  auto it = sockets.find(fd);
  if (it != sockets.end() && it->second.details_->pipe[0] == fd) {
    return it->second;
  }

  return Socket();
}

bool Socket::SetState(State state, const struct sockaddr* address,
                      socklen_t size) {
  switch (state) {
    case IDLE:
      return details_->state == IDLE;

    case CONNECTING: {
      if (details_->state == IDLE) {
        if (!address || size > sizeof(details_->address)) {
          errno = EINVAL;
          break;
        }
        // TODO: check that |addr| corresponds to the socket type.
        // TODO: check that |addr| is not already in use.
        memcpy(&details_->address, address, size);
        details_->state = state;
        return true;
      } else if (details_->state == CONNECTING) {
        errno = EALREADY;
      } else if (details_->state >= CONNECTED) {
        errno = EISCONN;
      }
    } break;

    case CONNECTED: {
      if (details_->state == CONNECTING) {
        details_->state = state;
        return true;
      } else {
        errno = EINVAL;
      }
    } break;

    case BIND: {
      if (details_->state == IDLE) {
        if (!address || size > sizeof(details_->address)) {
          errno = EINVAL;
          break;
        }
        // TODO: check that |addr| corresponds to the socket type.
        // TODO: handle some AF_UNIX-specific problems.
        memcpy(&details_->address, address, size);
        details_->state = state;
        return true;
      } else {
        errno = EINVAL;
      }
    } break;

    case PASSIVE: {
      if (details_->state == BIND) {
        // TODO: check that |addr| is not already in use.
        details_->state = state;
        return true;
      } else {
        errno = EOPNOTSUPP;
        return false;
      }
    } break;
  }

  return false;
}

int Socket::Close() {
  if (!details_->is_valid) {
    errno = EBADF;
    return -1;
  }

  UniqueLock lock(sockets_mutex);

  sockets.erase(details_->pipe[0]);
  sockets.erase(details_->pipe[1]);

  return Mock::CallOriginal<int>("close", details_->pipe[0]) |
         Mock::CallOriginal<int>("close", details_->pipe[1]);
}

}  // namespace mock_network
