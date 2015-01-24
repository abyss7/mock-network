#pragma once

#include <base/aliases.h>

namespace mock_network {

extern bool Enable(String* error = nullptr);
extern bool Disable(String* error = nullptr);

}  // namespace mock_network
