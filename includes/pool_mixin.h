#pragma once

#include "memory_pool.h"

template <typename T, std::size_t Size> class PoolMixin {
public:
  static inline MemoryPool<T> pool{Size};

public:
  void *operator new(std::size_t _size) { return pool.get(); };

  void operator delete(void *p) noexcept {
    bool result = pool.put((T *)p);
    if (!result) {
      // exception to failed delete object
    }
  };
};
