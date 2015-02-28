#pragma once

#include <base/aliases.h>

#include <cassert>

namespace mock_network {

class Mock {
 public:
  template <typename R, typename... Args>
  static R CallOriginal(const String& name, Args... args) {
    // FIXME: implement my own |CHECK()| and |DCHECK()|.
    auto original = originals_.find(name);
    assert(enabled_);
    assert(original != originals_.end());
    return reinterpret_cast<R (*)(Args...)>(original->second)(args...);
  }

  static bool Enable(String* error = nullptr);
  static bool Disable(String* error = nullptr);

 private:
  static Mutex mutex_;
  static bool enabled_;
  static Map<String, void*> originals_;
};

}  // namespace mock_network
