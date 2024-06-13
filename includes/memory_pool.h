#pragma once
#include <iostream>

#include "exceptions.h"
#include "logger.h"
#include <cstddef>
#include <iostream>
#include <memory>
#include <string>
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
  // values
private:
  int size_;
  std::vector<T> free_memory_;

public:
  // Default constructor, does not allocate any memory
  MemoryPool() = default;

  // Allocates memory for given number of objects
  MemoryPool(size_t _capacity) : size_{0} {
    free_memory_ = std::vector<T>(_capacity);
  };

  // Allocates memory for given number of objects and initializes them with
  // `_default_value`
  MemoryPool(std::size_t _capacity, const T &_default_value) : size_{0} {
    free_memory_ = std::vector<T>(_capacity, _default_value);
  };

  ~MemoryPool() { free_memory_.clear(); }

  // Re-allocates memory to fit new capacity
  void resize(size_t new_cap) {
    Logger::get_instance()->log(
        LogLevel::INFO, "RESIZE memory pool " + std::to_string(size_ - free_memory_.size()) +
                            " -> " + std::to_string(new_cap));
    size_ = new_cap;
    free_memory_.resize(new_cap);
  };

  // Creates object inside memory pool and provides pointer to it can return
  // nullptr in case if memory cannot be allocated.
  template <class... Args> T *get(Args &&...args) {
    if (!free_memory_.size()) {
      return nullptr;
    }

    auto &cell = free_memory_.pop_back();

    cell = T(std::forward<Args>(args)...);
    Logger::get_instance()->log(LogLevel::INFO,
                                "GET new object from memory pool, size: " +
                                    std::to_string(size_));

    return &cell;
  };

  // Returns object to pool, calls destructor.
  // Returns bool to indicate error (maybe object is not from this pool?)
  bool put(T *elem) {
    if (size_ == free_memory_.size()) {
      throw PoolIsEmpty();
    }
    elem->~T();
    free_memory_.push_back(T());
    Logger::get_instance()->log(LogLevel::INFO,
                                "PUT object into memory pool, pool size: " +
                                    std::to_string(size_));

    return true;
  };

  // Creates object in memory pool and provides unique pointer to it
  template <class... Args> std::unique_ptr<T, D<T>> unique(Args &&...args) {
    Logger::get_instance()->log(LogLevel::INFO,
                                "request to get UNIQUE ptr from pool");

    T *elem = this->get(std::forward<Args>(args)...);
    return std::unique_ptr<T, D<T>>(elem, D<T>());
  };

  // Creates object in memory pool and provides shared pointer to it
  template <class... Args> std::shared_ptr<T> shared(Args &&...args) {
    Logger::get_instance()->log(LogLevel::INFO,
                                "request to get SHARED ptr from pool");

    return std::shared_ptr<T>(this->get(std::forward<Args>(args)...));
  };
};
