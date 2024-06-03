#include <iostream>

struct Deleter {};

template <typename ElemT> class MemPool {
public:
  // Default constructor, does not allocate any memory
  MemPool<ElemT>();

  // Allocates memory for given number of objects
  MemPool<ElemT>(std::size_t _capacity);

  // Allocates memory for given number of objects and initializes them with
  // `_default_value`
  MemPool<ElemT>(std::size_t _capacity, const ElemT &_default_value);

  // Re-allocates memory to fit new capacity
  //void resize(size_t new_cap);

  // Creates object inside memory pool and provides pointer to it
  // can return nullptr in case if memory cannot be allocated.
  //template <class... Args> ElemT *get(Args &&...args);

  // Returns object to pool, calls destructor.
  // Returns bool to indicate error (maybe object is not from this pool?)
  //bool put(ElemT *elem);

  // Creates object in memory pool and provides unique pointer to it
  //template <class... Args>
  //std::unique_ptr<ElemT, Deleter> unique(Args &&...args);

  // Creates object in memory pool and provides shared pointer to it
  //template <class... Args> std::shared_ptr<ElemT> shared(Args &&...args);

  // values
  int capacity;
  ElemT value;
};
