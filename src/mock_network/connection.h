#pragma once

#include <mock_network/functions.h>

#include <chrono>

namespace mock_network {

class Connection {
 public:
  static void SetDuration(std::chrono::seconds duration);
  static void SetResult(int error_code);

 private:
  friend int fake::connect(int sockfd, const struct sockaddr* addr,
                           socklen_t addrlen);

  static std::chrono::seconds duration_;
  static int result_;
};

}  // namespace mock_network
