#include "../src/memory_pool.cpp"
#include <cstddef>
#include <iostream>

struct Test;

// MemPool<Test> pool(15);

// PoolMixin to integrate new and deleter operators to TestClass
template <typename T, std::size_t Size> class PoolMixin {
public:
  static inline MemoryPool<T> pool{Size};

public:
  void *operator new(std::size_t _size) { return pool.get(); };

  void operator delete(void *p) noexcept {
    bool result = pool.put((Test *)p);
    if (!result) {
      // exception to failed delete object
    }
  };
};

// Test structure for testing MemoryPool
struct Test : PoolMixin<Test, 10> {
public:
  Test() : a{0}, b{0} {};
  Test(int _a, int _b) : a{_a}, b{_b} {};

  using PoolMixin<Test, 10>::operator new;
  using PoolMixin<Test, 10>::operator delete;

  int a, b;
};

// printing the memory pool

int main() {
  std::cout << "Created memory pool with size: 15\n";

  std::size_t objects_count = 5;
  for (int i = 0; i < objects_count; i++) {
    std::shared_ptr<Test> test = PoolMixin<Test, 10>::pool.shared(10, 20);
    Test *test2 = new Test(68, 91);
  }

  PoolMixin<Test, 10>::pool.resize(20);
  return 0;
}
