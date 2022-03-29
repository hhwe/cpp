#ifndef MYSTL_LIST_H_
#define MYSTL_LIST_H_

#include "allocator.h"
#include "construct.h"

namespace MyStl {

template <typename T, typename Alloc = MyStl::allocator<T>>
class list {
public:
    using value_type = T;
    using pointer = T*;
    using const_pointer = const T*;
    using reference = T&;
    using const_reference = const T&;
    using size_type = std::size_t;
    using difference_type = std::ptrdiff_t;

    using iterator = value_type*;
    using const_iterator = const value_type*;
    using data_allocator = Alloc;

protected:
    iterator first_;
    iterator finish_;
    iterator capacity_;
};

} // namespace MyStl

#endif // MYSTL_LIST_H_
