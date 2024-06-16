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
  size_t size_;                  // contains max memory pool size
  std::vector<T*> free_memory_;  // stack with free momory
                                
  // =========
  void print_memory(){
    for (T* elem: free_memory_){
      std::cout << elem << "\n";
    }
    std::cout << "---\n\n";
  }
  // =========

public:
  // Default constructor, does not allocate any memory
  MemoryPool() = default;

  // Allocates memory for given number of objects
  MemoryPool(size_t _capacity) : size_{_capacity} {
    //free_memory_ = std::vector<T*>(_capacity, T());
    for (int i = 0; i < _capacity; i++) {
      free_memory_.push_back(::new T());
    }
    print_memory();
  };

  // Allocates memory for given number of objects and initializes them with
  // `_default_value`
  MemoryPool(std::size_t _capacity, const T &_default_value)
      : size_{_capacity} {
    //free_memory_ = std::vector<T*>(_capacity, nullptr);
    for (int i = 0; i < size_; i++) {
      free_memory_.push_back(::new T());
    }

    print_memory();
  };

  ~MemoryPool() {
    free_memory_.clear();
  }


  // Re-allocates memory to fit new capacity
  void resize(size_t new_cap) {
    Logger::get_instance()->log(
        LogLevel::INFO, "RESIZE memory pool " +
                            std::to_string(size_ - free_memory_.size()) +
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
    T *cell = free_memory_.back();
    free_memory_.pop_back();

    // putting memory into vector with allocated memory
    *cell = T(std::forward<Args>(args)...);

    Logger::get_instance()->log(LogLevel::INFO,
                                "GET new object from memory pool, size: " +
                                    std::to_string(free_memory_.size()));

    return cell;
  };

  // Returns object to pool, calls destructor.
  // Returns bool to indicate error (maybe object is not from this pool?)
  bool put(T *elem) {
    if (size_ == free_memory_.size()) {
      throw PoolIsEmpty();
    }

    //std::cout << "PUT, begin: " << *free_memory_.begin() << ", elem: " << elem << ", end: " << *free_memory_.begin() + size_ * sizeof(T) << "\n"; 
    if (*free_memory_.begin() <= elem && elem <= *free_memory_.begin() + size_ * sizeof(T)) {
      elem->~T();
      free_memory_.push_back(elem);
      Logger::get_instance()->log(LogLevel::INFO,
                                  "Successfull PUT, pool size: " +
                                      std::to_string(free_memory_.size()));
      return true;
    }

    Logger::get_instance()->log(LogLevel::INFO,
                                "Failed PUT, pool size: " +
                                    std::to_string(free_memory_.size()));
    return false;
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
