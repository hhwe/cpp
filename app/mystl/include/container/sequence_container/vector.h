#ifndef MYSTL_VECTOE_H_
#define MYSTL_VECTOE_H_

#include <iostream>
#include "allocator.h"
#include "construct.h"
#include "uninitialized.h"

namespace mystl {

template <typename T, typename Alloc = mystl::allocator<T>>
class vector {
public: // member types
    using allocator_type = Alloc;
    using data_allocator = Alloc;

    using value_type = typename allocator_type::value_type;
    using pointer = typename allocator_type::pointer;
    using const_pointer = typename allocator_type::const_pointer;
    using reference = typename allocator_type::reference;
    using const_reference = typename allocator_type::const_reference;
    using size_type = typename allocator_type::size_type;
    using difference_type = typename allocator_type::difference_type;

    using iterator = value_type*;
    using const_iterator = const value_type*;
    using reverse_iterator = mystl::reverse_iterator<iterator>;
    using const_reverse_iterator = mystl::reverse_iterator<const_iterator>;

public: // member functions
    /*
     * Constructor and Destructor
     */
    vector() :
        begin_(0), end_(0), capacity_(0) {
    }

    vector(size_type n, const_reference val) {
        fill_initialize(n, val);
    }

    vector(size_type n) {
        fill_initialize(n, T());
    }

    ~vector() {
        data_allocator::destroy(begin_, end_);
        data_allocator::deallocate(begin_, capacity_ - end_);
    }

    /*
     * Iterators
     */
    iterator begin() noexcept {
        return begin_;
    }

    iterator end() noexcept {
        return end_;
    }

    const_iterator begin() const noexcept {
        return begin_;
    }

    const_iterator end() const noexcept {
        return end_;
    }

    reverse_iterator rbegin() noexcept {
        return reverse_iterator(end());
    }

    reverse_iterator rend() noexcept {
        return reverse_iterator(begin());
    }

    const_reverse_iterator rbegin() const noexcept {
        return end();
    }

    const_reverse_iterator rend() const noexcept {
        return begin();
    }

    const_iterator cbegin() const noexcept {
        return begin();
    }

    const_iterator cend() const noexcept {
        return end();
    }

    const_reverse_iterator crbegin() const noexcept {
        return rbegin();
    }

    const_reverse_iterator crend() const noexcept {
        return rend();
    }
    /*
     * Capacity
     */
    size_type size() const noexcept {
        return static_cast<size_type>(capacity_ - begin_);
    }

    size_type max_size() const noexcept {
        return static_cast<size_type>(-1) / sizeof(value_type);
    }

    void resize(size_type n) {
        if (n < size()) {
            data_allocator::destroy(begin_ + n, end_);
            end_ = begin_ + n;
        } else if (n > size() && n <= capacity()) {
            // TODO:
        }
    }

    void resize(size_type n, const value_type& val) {
    }

    size_type capacity() const noexcept {
        return size_type(capacity_ - begin_);
    }

    bool empty() const noexcept {
        return begin_ == end_;
    }

    void reserve(size_type n) {
        // TODO:
    }

    void shrink_to_fit();

    /*
     * Element access
     */
    reference operator[](size_type n) {
        return *(begin_ + n);
    }

    const_reference operator[](size_type n) const {
        return *(begin_ + n);
    }

    reference at(size_type n) {
        return (*this)[n];
    }

    const_reference at(size_type n) const {
        return (*this)[n];
    }

    reference front() {
        return *(begin_);
    }

    const_reference front() const {
        return *(begin_);
    }

    reference back() {
        return *(end_ - 1);
    }

    const_reference back() const {
        return *(end_ - 1);
    }

    pointer data() noexcept {
        return begin_;
    }

    const_pointer data() const noexcept {
        return begin_;
    }

    /*
     * Modifiers
     */
    template <class InputIterator>
    void assign(InputIterator first, InputIterator last);

    void assign(size_type n, const value_type& val);

    // void assign(initializer_list<value_type> il);

    void push_back(const value_type& val) {
        insert_aux(end_, val);
    }

    void push_back(value_type&& val) {
        emplace_back(mystl::move(val));
    }

    void pop_back() {
        --end_;
        data_allocator::destroy(end_);
    }

    iterator insert(iterator position, const value_type& val) {
        return insert_aux(position, val);
    }

    // iterator insert(const_iterator position, size_type n, const value_type& val);
    // template <class InputIterator>
    // iterator insert(const_iterator position, InputIterator first, InputIterator last);
    // iterator insert(const_iterator position, value_type&& val);
    // iterator insert(const_iterator position, initializer_list<value_type> il);

    iterator erase(iterator position) {
        return erase(position, position + 1);
    }

    iterator erase(iterator first, iterator last) {
        mystl::move(last, end_, first);
        auto new_end = end_ - (first - last);
        data_allocator::destroy(new_end, end_);
        end_ = new_end;
        return first;
    }

    void swap(vector& rhs) {
        if (this != &rhs) {
            mystl::swap(begin_, rhs.begin_);
            mystl::swap(end_, rhs.end_);
            mystl::swap(capacity_, rhs.capacity_);
        }
    }

    void clear() {
        erase(begin_, end_);
    }

    template <class... Args>
    iterator emplace(iterator position, Args&&... args) {
        return emplace_aux(position, mystl::forward<Args>(args)...);
    }

    template <class... Args>
    void emplace_back(Args&&... args) {
        emplace_aux(end_, mystl::forward<Args>(args)...);
    }

private:
    void fill_initialize(size_type n, const_reference val) {
        begin_ = data_allocator::allocate(n);
        mystl::uninitialized_fill_n(begin_, n, val);
        end_ = begin_ + n;
        capacity_ = end_;
    }

    void realloc_insert(iterator pos, const_reference val) {
        const size_type len = size() != 0 ? 2 * size() : 1;
        iterator new_begin = data_allocator::allocate(len);
        iterator new_end = new_begin;
        try {
            new_end = mystl::uninitialized_copy(begin_, pos, new_begin);
            data_allocator::construct(new_end, val);
            ++new_end;
            new_end = mystl::uninitialized_copy(pos, end_, new_end);
        } catch (...) {
            data_allocator::destroy(new_begin, new_end);
            data_allocator::deallocate(new_begin, len);
            throw;
        }
        data_allocator::destroy(begin_, end_);
        data_allocator::deallocate(begin_, capacity_ - begin_);

        begin_ = new_begin;
        end_ = new_end;
        capacity_ = new_begin + len;
    }

    iterator insert_aux(iterator pos, const_reference val) {
        const auto n = pos - begin_;
        if (end_ != capacity_) {
            if (pos == end_) {
                data_allocator::construct(end_, val);
                ++end_;
            } else {
                auto new_end = end_;
                data_allocator::construct(end_, val);
                ++new_end;
                mystl::copy_backward(pos, end_ - 1, end_);
                *pos = val;
                end_ = new_end;
            }
        } else {
            realloc_insert(pos, val);
        }
        return begin_ + n;
    }

    template <class... Args>
    void realloc_emplace(iterator pos, Args&&... args) {
        const size_type len = size() != 0 ? 2 * size() : 1;
        iterator new_begin = data_allocator::allocate(len);
        iterator new_end = new_begin;
        try {
            new_end = mystl::uninitialized_move(begin_, pos, new_begin);
            data_allocator::construct(new_end, mystl::forward<Args>(args)...);
            ++new_end;
            new_end = mystl::uninitialized_move(pos, end_, new_end);
        } catch (...) {
            data_allocator::destroy(new_begin, new_end);
            data_allocator::deallocate(new_begin, len);
            throw;
        }
        data_allocator::destroy(begin_, end_);
        data_allocator::deallocate(begin_, capacity_ - begin_);

        begin_ = new_begin;
        end_ = new_end;
        capacity_ = new_begin + len;
    }

    template <class... Args>
    iterator emplace_aux(iterator pos, Args&&... args) {
        const auto n = pos - begin_;
        if (end_ != capacity_) {
            if (pos == end_) {
                data_allocator::construct(end_, mystl::forward<Args>(args)...);
                ++end_;
            } else {
                auto new_end = end_;
                data_allocator::construct(end_, *(end_ - 1));
                ++new_end;
                mystl::copy_backward(pos, end_ - 1, end_);
                *pos = value_type(mystl::forward<Args>(args)...);
                end_ = new_end;
            }
        } else {
            realloc_emplace(pos, mystl::forward<Args>(args)...);
        }
        return begin_ + n;
    }

protected:
    iterator begin_;
    iterator end_;
    iterator capacity_;
};

} // namespace mystl

#endif // MYSTL_VECTOE_H_
