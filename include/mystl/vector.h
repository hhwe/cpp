#ifndef MYSTL_VECTOE_H_
#define MYSTL_VECTOE_H_
#include "allocator.h"

namespace mystl {

template <typename T, typename Alloc = mystl::allocator<T>>
class vector {
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

    // Iterators
    iterator begin() {
        return start;
    }
    iterator end() {
        return finish;
    }

    // Capacity
    size_type size() const {
        return size_type(finish - start);
    }
    size_type capacity() const {
        return size_type(end_of_storage - start)
    }
    bool empty() const {
        return start == finish;
    }

    // Element access
    reference operator[](size_type n) {
        return *(start + n);
    }
    reference at(size_type n) {
        return *(start + n);
    }
    reference front() {
        return *(start);
    }
    reference back() {
        return *(finish);
    }

    // Modifiers
    void push_back(const value_type& val) {
    }

    void fill_initialize(size_type n, const_reference val) {
        start = Alloc::allocate(n);
        std::uninitialized_fill_n(result, n, x);
        finish = start + n;
        end_of_storage = finish;
    }

    vector() :
        start(0), finish(0), end_of_storage(0) {
    }
    vector(size_type n, const_reference val) {
        fill_initialize(n, val);
    }
    explicit vector(size_type n) {
        fill_initialize(n, T());
    }

    ~vector() {
        Alloc::destroy(start, finish);
        Alloc::deallocate(start, finish);
    }

protected:
    iterator start;
    iterator finish;
    iterator end_of_storage;
};

} // namespace mystl

#endif // MYSTL_VECTOE_H_
