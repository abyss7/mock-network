#include <mock_network/mock.h>

#include <base/c_utils.h>
#include <mock_network/functions.h>
#include <third_party/elf_hook/exported/elf_hook.h>

#include <regex>

#include <link.h>

namespace mock_network {

namespace {

struct link_map_unloader {
  void operator()(struct link_map* handle) const { dlclose(handle); }
};

using LinkMap = UniquePtr<struct link_map, link_map_unloader>;
using LinkList = List<LinkMap>;

LinkList GetLoadedModules() {
  // Taken from http://goo.gl/4xwAHz
  using UnknownStruct = struct unknown_struct {
    void* unused_pointers[3];
    struct unknown_struct* ptr;
  };

  auto main = reinterpret_cast<UnknownStruct*>(dlopen(nullptr, RTLD_NOW));
  auto map = reinterpret_cast<struct link_map*>(main->ptr->ptr);

  LinkList modules;
  while (map) {
    auto* handle = dlopen(map->l_name, RTLD_LAZY);
    modules.emplace_back(reinterpret_cast<struct link_map*>(handle));
    map = map->l_next;
  }

  // FIXME: check returned value.
  dlclose(main);

  return std::move(modules);
}

const char* blacklist[] = {
    // Old libstdc++ doesn't support substring regex matching.
    ".*ld-linux-x86-64\\.so.*", ".*libc\\.so.*", ".*libpthread\\.so.*",
};

Map<String, void*> handles;

}  // namespace

// static
bool Mock::Enable(String* error) {
  UniqueLock lock(mutex_);

  if (enabled_) {
    if (error) {
      error->assign("Mocking is already enabled");
    }
    return false;
  }

  enabled_ = true;

  LinkList modules = GetLoadedModules();

  for (const auto& handle : modules) {
    bool in_blacklist = false;
    for (const auto* entry : blacklist) {
      if (std::regex_match(handle->l_name, std::regex(entry))) {
        in_blacklist = true;
        break;
      }
    }
    if (in_blacklist) {
      continue;
    }

    void* base = nullptr;
    if (get_module_base_address(handle->l_name, handle.get(), &base)) {
      String prefix =
          String("Failed to get base address for ") + handle->l_name;
      if (base != nullptr) {
        base::GetLastError(error, prefix);
      } else if (error) {
        error->assign(prefix + ": address is null");
      }
      return false;
    }

#define MOCK(func)                                                        \
  {                                                                       \
    void* original = elf_hook(handle->l_name, base, #func,                \
                              reinterpret_cast<const void*>(fake::func)); \
    if (original) {                                                       \
      originals_.emplace(#func, original);                                \
    }                                                                     \
  }

    MOCK(accept);
    MOCK(bind);
    MOCK(close);
    MOCK(connect);
    MOCK(listen);
    MOCK(setsockopt);
    MOCK(socket);

#undef MOCK

    handles.emplace(handle->l_name, base);
  }

  return true;
}

// static
bool Mock::Disable(String* error) {
  UniqueLock lock(mutex_);

  if (!enabled_) {
    if (error) {
      error->assign("Mocking is not enabled");
    }
    return false;
  }

  for (const auto& handle : handles) {
#define RESTORE(func)                                                         \
  {                                                                           \
    auto original = originals_.find(#func);                                   \
    if (original != originals_.end()) {                                       \
      elf_hook(handle.first.c_str(), handle.second, #func, original->second); \
    }                                                                         \
  }

    RESTORE(accept);
    RESTORE(bind);
    RESTORE(close);
    RESTORE(connect);
    RESTORE(listen);
    RESTORE(setsockopt);
    RESTORE(socket);

#undef RESTORE
  }

  originals_.clear();
  handles.clear();

  // NOTE: |elf_hook()| uses the |close()| internally, so we need to be able to
  //       call the |CallOriginal()|, which asserts the |enabled_|.
  enabled_ = false;

  return true;
}

// static
Mutex Mock::mutex_;

// static
bool Mock::enabled_ = false;

// static
Map<String, void*> Mock::originals_;

}  // namespace mock_network
