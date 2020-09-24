#pragma once
#include <atomic>

namespace hammerblade {
template <typename T>
struct lockable {
  std::atomic<int> lock;
  T value;

  lockable():lock(0){}
};
};
