#pragma once

#include <base/aliases.h>

#include <cerrno>
#include <cstring>

namespace mock_network {
namespace base {

void GetLastError(String* error, String prefix = String()) {
  if (error) {
    error->assign(strerror(errno));
    if (!prefix.empty()) {
      error->assign(prefix + ": " + *error);
    }
  }
}

}  // namespace base
}  // namespace mock_network
