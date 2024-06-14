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
  size_t size_; // contains max memory pool size 
  std::vector<T> free_memory_; // stack with free momory
  std::vector<T*> allocated_memory_; // vector with allocated memory

public:
  // Default constructor, does not allocate any memory
  MemoryPool() = default;

  // Allocates memory for given number of objects
  MemoryPool(size_t _capacity) : size_{_capacity} {
    free_memory_ = std::vector<T>(_capacity);
  };

  // Allocates memory for given number of objects and initializes them with
  // `_default_value`
  MemoryPool(std::size_t _capacity, const T &_default_value) : size_{_capacity} {
    free_memory_ = std::vector<T>(_capacity, _default_value);
  };

  ~MemoryPool() {
    free_memory_.clear();
    allocated_memory_.clear();
  }

  // Re-allocates memory to fit new capacity
  void resize(size_t new_cap) {
    Logger::get_instance()->log(
        LogLevel::INFO, "RESIZE memory pool " + std::to_string(size_ - free_memory_.size()) +
                            " -> " + std::to_string(new_cap));
    size_ = new_cap;
    free_memory_.resize(free_memory_.size() + new_cap - size_);
  };

  // Creates object inside memory pool and provides pointer to it can return
  // nullptr in case if memory cannot be allocated.
  template <class... Args> T *get(Args &&...args) {
    if (!free_memory_.size()) {
      return nullptr;
    }

    // getting memory from 'free memory' stack
    auto &cell = free_memory_[free_memory_.size() - 1];
    free_memory_.pop_back();

    std::cout << "auto &cell - contains:" << &cell << std::endl;

    // putting memory into vector with allocated memory 
    cell = T(std::forward<Args>(args)...);
    allocated_memory_.push_back(&cell);

    Logger::get_instance()->log(LogLevel::INFO,
                                "GET new object from memory pool, size: " +
                                    std::to_string(free_memory_.size()));

    return &cell;
  };

  // Returns object to pool, calls destructor.
  // Returns bool to indicate error (maybe object is not from this pool?)
  bool put(T *elem) {
    std::cout << "T *elem contains: " << elem << std::endl;
    if (size_ == free_memory_.size()) {
      throw PoolIsEmpty();
    }

    bool found;
    for (int i = 0; i < allocated_memory_.size(); i++){
      T* cell_ptr = allocated_memory_[i];

      if (cell_ptr == elem){
        found = true;
        elem->~T();

        free_memory_.push_back(*elem);
        allocated_memory_.erase(allocated_memory_.begin() + i);
        break;
      }
    }


    if (found){
      Logger::get_instance()->log(LogLevel::INFO,
                                "PUT object into memory pool, pool size: " +
                                    std::to_string(free_memory_.size()));
    }else{
      Logger::get_instance()->log(LogLevel::INFO, "failed to put object into pool");
    }

    return found;
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
