#include "TeamSet.h"

template <typename T, typename Compare = std::less<T>>
struct DereferencePtrLess {
  Compare cmp;

  
  DereferencePtrLess() {}
  DereferencePtrLess(Compare c) : cmp(std::move(c)) {}

  bool operator()(T const* a, T const* b) const {
    return cmp(*a, *b);
  }
};
