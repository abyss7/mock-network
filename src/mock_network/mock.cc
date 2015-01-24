#include <mock_network/mock.h>

#include <base/c_utils.h>
#include <mock_network/functions.h>
#include <third_party/elf_hook/exported/elf_hook.h>

#include <regex>

#include <link.h>

namespace mock_network {

// Used in the fake |close()|.
Map<String, void*> originals;

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

Atomic<bool> enabled = {false};

#define REPLACE(func)                                     \
  originals.emplace(#func,                                \
                    elf_hook(handle->l_name, base, #func, \
                             reinterpret_cast<const void*>(fake::func)))

const char* blacklist[] = {
    "ld-linux-x86-64\\.so", "libc\\.so", "libpthread\\.so",
};

}  // namespace

bool Enable(String* error) {
  bool expected = false;
  if (!enabled.compare_exchange_strong(expected, true)) {
    if (error) {
      error->assign("Mocking is already enabled");
    }
    return false;
  }

  LinkList modules = GetLoadedModules();

  for (auto& handle : modules) {
    bool in_blacklist = false;
    for (const auto* entry : blacklist) {
      if (std::regex_search(handle->l_name, std::regex(entry))) {
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

    REPLACE(close);
    REPLACE(connect);
    REPLACE(socket);
  }

  return true;
}

bool Disable(String* error) {
  bool expected = true;
  if (!enabled.compare_exchange_strong(expected, false)) {
    if (error) {
      error->assign("Mocking is not enabled");
    }
    return false;
  }

  // TODO: implement this.
  return true;
}

}  // namespace mock_network
