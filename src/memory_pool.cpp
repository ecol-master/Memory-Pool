#include "../includes/memory_pool.h"
#include <cstddef>
#include <iostream>
#include <memory>
#include <vector>

// Deleter for Objetcs
template <typename T> struct D {
  D() {};
  D(const D &) { std::cout << "D copy ctor\n"; }
  D(D &) { std::cout << "D non-const copy ctor\n"; }
  D(D &&) { std::cout << "D move ctor \n"; }
  void operator()(T *p) const {
    std::cout << "D is deleting a Foo\n";
    delete p;
  };
};

// Memory Pool
template <typename T> class MemoryPool {
public:
  // Default constructor, does not allocate any memory
  MemoryPool() = default;

  // Allocates memory for given number of objects
  MemoryPool(size_t _capacity) {
    data_ = std::vector<T>(_capacity);
    pool_size_ = 0;
  };

  // Allocates memory for given number of objects and initializes them with
  // `_default_value`
  MemoryPool(std::size_t _capacity, const T &_default_value) {
    data_ = std::vector<T>(_capacity, _default_value);
    pool_size_ = 0;
  };

  ~MemoryPool() { data_.clear(); }

  // Re-allocates memory to fit new capacity
  void resize(size_t new_cap) { data_.resize(new_cap); };

  // Creates object inside memory pool and provides pointer to it can return
  // nullptr in case if memory cannot be allocated.
  template <class... Args> T *get(Args &&...args) {
    std::cout << "get object from pool\n";
    if (pool_size_ == data_.size()) {
      return nullptr;
    }

    auto &cell = data_[pool_size_];
    pool_size_++;

    cell = T(std::forward<Args>(args)...);
    return &cell;
  };

  // Returns object to pool, calls destructor.
  // Returns bool to indicate error (maybe object is not from this pool?)
  bool put(T *elem) {
    std::cout << "put object into pool\n";
    if (pool_size_ == 0) {
      throw PoolIsEmpty();
      // throw exception
    }
    pool_size_--;
    elem->~T();
    data_[pool_size_] = T();
    return true;
  };

  // Creates object in memory pool and provides unique pointer to it
  template <class... Args> std::unique_ptr<T, D<T>> unique(Args &&...args) {
    T *elem = this->get(std::forward<Args>(args)...);
    D<T> deleter;
    return std::unique_ptr<T, D<T>>(elem, deleter);
  };

  // Creates object in memory pool and provides shared pointer to it
  template <class... Args> std::shared_ptr<T> shared(Args &&...args) {
    return std::shared_ptr<T>(this->get(std::forward<Args>(args)...));
  };

  // values
private:
  int pool_size_;
  std::vector<T> data_;
};
