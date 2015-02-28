#include <mock_network/connection.h>

namespace mock_network {

// static
void Connection::SetDuration(std::chrono::seconds duration) {
  duration_ = duration;
}

// static
void Connection::SetResult(int error_code) {
  result_ = error_code;
}

// static
std::chrono::seconds Connection::duration_(0);

// static
int Connection::result_ = 0;

}  // namespace mock_network
