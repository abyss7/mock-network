#pragma once

#include <chrono>

namespace mock_network {

void SetConnectionDuration(std::chrono::seconds duration);
void SetConnectionResult(int error_code);

}  // namespace mock_network
