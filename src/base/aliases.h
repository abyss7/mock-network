#pragma once

#include <atomic>
#include <list>
#include <memory>
#include <mutex>
#include <set>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

template <class T>
using Atomic = std::atomic<T>;

template <class T>
using List = std::list<T>;

using String = std::string;

template <class U, class V>
using Map = std::unordered_map<U, V>;

using Mutex = std::mutex;

template <class T>
using Set = std::set<T>;

template <class T>
using SharedPtr = std::shared_ptr<T>;

using UniqueLock = std::unique_lock<Mutex>;

template <class U, class V>
using UniquePtr = std::unique_ptr<U, V>;

template <class U, class V>
using HashMap = std::unordered_map<U, V>;

template <class T>
using HashSet = std::unordered_set<T>;

template <class T>
using Vector = std::vector<T>;
