#include "../src/memory_pool.cpp"
#include <iostream>

struct Test;

// MemPool<Test> pool(15);

// PoolMixin to integrate new and deleter operators to TestClass
template <typename T> class PoolMixin {
public:
  static inline MemPool<T> pool{15};

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
struct Test : PoolMixin<Test> {
public:
  Test() : a{0}, b{0} {};
  Test(int _a, int _b) : a{_a}, b{_b} {};

  using PoolMixin<Test>::operator new;
  using PoolMixin<Test>::operator delete;

  int a, b;
};

// printing the memory pool
void print_memory_pool(MemPool<Test> pool) {
  for (int i = pool.size; i < pool.data.size(); i++) {
    std::cout << i << ") - {" << pool.data[i].a << ", " << pool.data[i].b << "}"
              << "\n";
  }
}

int main() {
  std::size_t pool_size = 15;
  // MemPool<Test> pool(pool_size, {0, 5});
  std::cout << "Created memory pool with size: " << pool_size << "\n";

  std::size_t objects_count = 5;
  for (int i = 0; i < objects_count; i++) {
    std::shared_ptr<Test> test = PoolMixin<Test>::pool.shared(10, 20);
    Test *test2 = new Test(68, 91);
  }

  // std::cout << "Pool size after creating " << objects_count << " objects - "
  //          << pool.data.size() - pool.size << "\n";

  //{
  // std::unique_ptr<Test, D<Test>> testU = pool.unique(100, 200);
  // std::cout << "Unique Ptr: {a: " << testU->a << ", b: " << testU->b <<
  // "}\n";
  //}

  std::cout << "Before resize: \n";
  print_memory_pool(PoolMixin<Test>::pool);

  PoolMixin<Test>::pool.resize(20);
  std::cout << "After resize: \n";
  print_memory_pool(PoolMixin<Test>::pool);
  return 0;
}
