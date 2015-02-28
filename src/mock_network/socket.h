#pragma once

#include <base/aliases.h>

#include <sys/socket.h>

namespace mock_network {

class Socket {
 public:
  enum State : unsigned {
    // The order of values is important.
    IDLE,
    CONNECTING,
    CONNECTED,
    BIND,
    PASSIVE,
  };

  Socket() = default;
  Socket(int domain, int type, int protocol);

  static Socket Get(int fd);

  inline void SetNonBlocking(bool value) { details_->non_blocking = value; }
  bool SetState(State state, const struct sockaddr* address = nullptr,
                socklen_t size = -1);

  inline bool IsValid() const { return details_->is_valid; }
  inline bool IsNonBlocking() const { return details_->non_blocking; }
  inline State GetState() const { return details_->state; }
  inline operator int() const {
    if (!IsValid()) {
      return -1;
    }
    return details_->pipe[0];
  }

  int Close();

 private:
  struct Details {
    // internals
    bool is_valid = false;
    int pipe[2];

    // socket params
    int domain, type, protocol;
    State state = IDLE;  // TODO: handle |state_| atomically.
    struct sockaddr_storage address;

    // file descriptor params
    bool non_blocking = false;
  };

  SharedPtr<Details> details_{new Details};
};

}  // namespace mock_network
