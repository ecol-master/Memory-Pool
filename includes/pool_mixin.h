#pragma once

#include "logger.h"
#include "memory_pool.h"

template <typename T, std::size_t Size> class PoolMixin {
public:
  static inline MemoryPool<T> pool{Size, {11, 12}};

public:
  void *operator new(std::size_t _size) { return pool.get(); };

  void operator delete(void *p) {
    bool result = pool.put((T *)p);
    if (!result) {
      Logger::get_instance()->log(LogLevel::WARNING,
                                  "failed to put object into pool");
    }
  };
};
