#include "../includes/pool_mixin.h"
#include <cstddef>


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
  Logger::get_instance()->log(LogLevel::INFO,
                              "Created memory pool with size: 10");

  auto a = new Test(70, 90);
  auto b = new Test(80, 90);
  PoolMixin<Test, 10>::pool.put(b);
  PoolMixin<Test, 10>::pool.put(a);

  std::vector<Test*> tests;

  for (int i = 0; i < 10; i++){
    tests.push_back(new Test(i, 10));
  }

  for (int i = 9; i >= 0; i--){
    delete tests[i];
  }


  std::size_t objects_count = 5;
  for (int i = 0; i < objects_count; i++) {
    std::shared_ptr<Test> test = PoolMixin<Test, 10>::pool.shared(10, 20);
    Logger::get_instance()->log(LogLevel::INFO,
                                "recieve shared ptr from pool, object: a: " +
                                    std::to_string(test->a) + " , b:" + std::to_string(test->b));
    Test *test2 = new Test(68, 91);
    Logger::get_instance()->log(LogLevel::INFO,
                                "recieve ptr from pool, object: a: " +
                                    std::to_string(test2->a) + " , b:" + std::to_string(test2->b));
  }

  PoolMixin<Test, 10>::pool.resize(20);
  return 0;
}
