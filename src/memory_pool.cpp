#include "../includes/memory_pool.h"
#include <cstddef>
#include <iostream>
#include <memory>
#include <vector>

// Deleter for Objetcs
template <typename ElemT> struct D {
  D() {};
  D(const D &) { std::cout << "D copy ctor\n"; }
  D(D &) { std::cout << "D non-const copy ctor\n"; }
  D(D &&) { std::cout << "D move ctor \n"; }
  void operator()(ElemT *p) const {
    std::cout << "D is deleting a Foo\n";
    delete p;
  };
};

// Memory Pool
template <typename ElemT> class MemPool {
public:
  // Default constructor, does not allocate any memory
  MemPool() = default;

  // Allocates memory for given number of objects
  MemPool(size_t _capacity) {
    data = std::vector<ElemT>(_capacity);
    size = 0;
  };

  // Allocates memory for given number of objects and initializes them with
  // `_default_value`
  MemPool(std::size_t _capacity, const ElemT &_default_value) {
    data = std::vector<ElemT>(_capacity, _default_value);
    size = 0;
  };

  // Re-allocates memory to fit new capacity
  void resize(size_t new_cap) { data.resize(new_cap); };

  // Creates object inside memory pool and provides pointer to it can return
  // nullptr in case if memory cannot be allocated.
  template <class... Args> ElemT *get(Args &&...args) {
    std::cout << "get object from pool\n";
    if (size == data.size()) {
      return nullptr;
    }

    auto &cell = data[size];
    size++;

    cell = ElemT(std::forward<Args>(args)...);
    return &cell;
  };

  // Returns object to pool, calls destructor.
  // Returns bool to indicate error (maybe object is not from this pool?)
  bool put(ElemT *elem) {
    std::cout << "put object into pool\n";
    if (size == 0) {
      throw PoolIsEmpty();
      // throw exception
    }
    size--;
    elem->~ElemT();
    data[size] = ElemT();
  };

  // Creates object in memory pool and provides unique pointer to it
  template <class... Args>
  std::unique_ptr<ElemT, D<ElemT>> unique(Args &&...args) {
    ElemT *elem = this->get(std::forward<Args>(args)...);
    D<ElemT> deleter;
    return std::unique_ptr<ElemT, D<ElemT>>(elem, deleter);
  };

  // Creates object in memory pool and provides shared pointer to it
  template <class... Args> std::shared_ptr<ElemT> shared(Args &&...args) {
    return std::shared_ptr<ElemT>(this->get(std::forward<Args>(args)...));
  };

  // values
  int size;
  std::vector<ElemT> data;
};
