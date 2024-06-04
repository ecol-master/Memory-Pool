#include <exception>
#include <iostream>

// Exceptions
class PoolOverFlow : std::exception {
public:
  std::string what() {
    return "can not allocate new memory because pool is overflow";
  }
};

class PoolIsEmpty : std::exception {
public:
  std::string what() { return "can not delete object because pool is empty"; }
};
