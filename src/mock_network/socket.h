#pragma once

#include <base/aliases.h>

namespace mock_network {

class Socket {
 public:
  enum State : unsigned {
    // The order of values is important.
    IDLE,
    CONNECTING,
    CONNECTED,
  };

  Socket() = default;
  Socket(int domain, int type, int protocol);

  static Socket Get(int fd);

  inline void SetNonBlocking(bool value) { non_blocking_ = value; }
  bool SetState(State state);

  inline bool IsValid() const { return is_valid_; }
  inline bool IsNonBlocking() const { return non_blocking_; }
  inline State GetState() const { return state_; }
  inline operator int() const {
    if (!IsValid()) {
      return -1;
    }
    return pipe_[0];
  }

  int Close();

 private:
  // internals
  bool is_valid_ = false;
  int pipe_[2];

  // socket params
  int domain_, type_, protocol_;
  State state_ = IDLE;  // TODO: handle |state_| atomically.

  // file descriptor params
  bool non_blocking_ = false;
};

}  // namespace mock_network
