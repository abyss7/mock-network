#include <mock_network/connect.h>

namespace mock_network {

std::chrono::seconds connection_duration(0);
int connection_result = 0;

void SetConnectionDuration(std::chrono::seconds duration) {
  connection_duration = duration;
}

void SetConnectionResult(int error_code) {
  connection_result = error_code;
}

}  // namespace mock_network
