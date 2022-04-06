#ifndef MYSTL_VECTOE_H_
#define MYSTL_VECTOE_H_

#include <iostream>
#include "allocator.h"
#include "construct.h"

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
    using data_allocator = Alloc;

    // Constructor and Destructor
    void fill_initialize(size_type n, const_reference val) {
        start_ = Alloc::allocate(n);
        std::uninitialized_fill_n(start_, n, val);
        finish_ = start_ + n;
        capacity_ = finish_;
    }

    vector() :
        start_(0), finish_(0), capacity_(0) {
    }

    vector(size_type n, const_reference val) {
        fill_initialize(n, val);
    }

    vector(size_type n) {
        fill_initialize(n, T());
    }

    ~vector() {
        Alloc::destroy(start_, finish_);
        Alloc::deallocate(start_, capacity_ - finish_);
    }

    // Iterators
    iterator begin() {
        return start_;
    }

    iterator end() {
        return finish_;
    }

    // Capacity
    size_type size() const {
        return size_type(finish_ - start_);
    }

    size_type capacity() const {
        return size_type(capacity_ - start_);
    }

    bool empty() const {
        return start_ == finish_;
    }

    // Element access
    reference operator[](size_type n) {
        return *(start_ + n);
    }

    reference at(size_type n) {
        return *(start_ + n);
    }

    reference front() {
        return *(start_);
    }

    reference back() {
        return *(finish_);
    }

    // Modifiers
    void realloc_insert(iterator pos, const_reference val) {
        const size_type len = size() != 0 ? 2 * size() : 1;
        iterator new_begin = Alloc::allocate(len);
        iterator new_end = new_begin;
        try {
            new_end = uninitialized_copy(start_, pos, new_begin); // TODO: use move
            construct(new_end, val);
            ++new_end;
            // new_end = uninitialized_copy(pos, finish_, new_end);
        } catch (const std::exception& e) {
            std::cerr << e.what() << '\n';
            destroy(new_begin, new_end);
            Alloc::deallocate(new_begin, len);
            throw;
        }
        destroy(start_, finish_);
        // deallocate(); TODO：

        start_ = new_begin;
        finish_ = new_end;
        capacity_ = new_begin + len;
    }

    void push_back(const value_type& val) {
        if (finish_ != capacity_) {
            construct(finish_, val);
            ++finish_;
        } else {
            realloc_insert(finish_, val);
        }
    }

    void pop_back() {
        --finish_;
        destroy(finish_);
    }

    iterator erase(iterator first, iterator last) {
        iterator i = copy(last, finish_, first);
    }

    iterator erase(iterator pos) {
        if (pos + 1 != finish_) {
            copy(pos + 1, finish_, pos);
        }
        --finish_;
        destroy(finish_);
        return pos;
    }

    void insert(iterator pos, size_type n, const T& x) {
        if (n <= 0) {
            return;
        }

        if (capacity_ - finish_ >= n) {
            size_type elems_after = finish_ - pos;
            // iterator
        }
    }

    void clear() {
        erase(start_, finish_);
    }

protected:
    iterator start_;
    iterator finish_;
    iterator capacity_;
};

} // namespace mystl

#endif // MYSTL_VECTOE_H_
