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
    // default (1)
    explicit vector(const allocator_type& alloc = allocator_type()) :
        begin_(0),
        end_(0), capacity_(0) {
    }
    // fill (2)
    explicit vector(size_type n) {
        fill_initialize(n, T());
    }
    vector(size_type n, const value_type& val, const allocator_type& alloc = allocator_type()) {
        fill_initialize(n, val);
    }
    // range (3)
    template <class InputIterator, typename = mystl::RequireInputIterator<InputIterator>>
    vector(InputIterator first, InputIterator last, const allocator_type& alloc = allocator_type()) {
        range_initialize(first, last);
    }

    // copy (4)
    vector(const vector& x) {
        range_initialize(x.begin_, x.end_);
    }
    vector(const vector& x, const allocator_type& alloc) {
        range_initialize(x.begin_, x.end_);
    }
    // move (5)
    vector(vector&& x) {
        begin_ = mystl::move(x.begin_);
        end_ = mystl::move(x.end_);
        capacity_ = mystl::move(x.capacity_);
    }
    vector(vector&& x, const allocator_type& alloc) {
        begin_ = mystl::move(x.begin_);
        end_ = mystl::move(x.end_);
        capacity_ = mystl::move(x.capacity_);
    }
    // initializer list (6)
    vector(std::initializer_list<value_type> il, const allocator_type& alloc = allocator_type()) {
        range_initialize(il.begin(), il.end());
    }

    ~vector() {
        destructor(begin_, end_, capacity_ - end_);
    }

    // copy (1)
    vector& operator=(const vector& x) {
        if (x != this) {
            const auto len = x.size();
            if (len > capacity()) {
                vector tmp(x);
                swap(tmp);
            } else if (len <= size()) {
                auto cur = mystl::copy(x.begin_, x.end_, begin_);
                data_allocator::destroy(cur, end_);
                end_ = cur;
            } else {
                mystl::copy(x.begin_, x.begin_ + size(), begin_);
                auto cur = mystl::uninitialized_copy(x.begin_ + size(), x.end_, end_);
                end_ = cur;
            }
        }
        return *this;
    }

    // move (2)
    vector& operator=(vector&& x) {
        destructor(begin_, end_, capacity_ - end_);
        begin_ = x.begin_;
        end_ = x.end_;
        capacity_ = x.capacity_;
        x.begin_ = nullptr;
        x.end_ = nullptr;
        x.capacity_ = nullptr;
        return *this;
    }

    // initializer list (3)
    vector& operator=(std::initializer_list<value_type> il) {
        vector tmp(il.begin(), il.end());
        swap(tmp);
        return *this;
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
        return static_cast<size_type>(end_ - begin_);
    }

    size_type max_size() const noexcept {
        return static_cast<size_type>(-1) / sizeof(value_type);
    }

    void resize(size_type n) {
        resize(n, value_type());
    }

    void resize(size_type n, const value_type& val) {
        if (n >= size()) {
            insert(end_, n - size(), val);
        } else {
            erase(begin_ + n, end_);
        }
    }

    size_type capacity() const noexcept {
        return size_type(capacity_ - begin_);
    }

    bool empty() const noexcept {
        return begin_ == end_;
    }

    void reserve(size_type n) {
        if (n > max_size()) {
            throw; // TODO
        }
        if (n > capacity()) {
            const size_type old_size = size();
            iterator new_begin = data_allocator::allocate(n);
            mystl::uninitialized_move(begin_, end_, new_begin);
            destructor(begin_, end_, capacity());
            begin_ = new_begin;
            end_ = new_begin + old_size;
            capacity_ = begin_ + n;
        }
    }

    void shrink_to_fit() {
        if (end_ < capacity_) {
            vector tmp(begin_, end_);
            swap(tmp);
        }
    }

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
    template <class InputIterator, typename = mystl::RequireInputIterator<InputIterator>>
    void assign(InputIterator first, InputIterator last) {
        assign_dispatch(first, last, std::false_type());
    }
    void assign(size_type n, const value_type& val) {
        fill_assign(n, val);
    }
    void assign(std::initializer_list<value_type> il) {
        assign_dispatch(il.begin(), il.end(), std::false_type());
    }

    void push_back(const value_type& val) {
        insert_aux(end_, val); // Optimize: use emplace_back
    }

    void push_back(value_type&& val) {
        emplace_back(mystl::move(val));
    }

    void pop_back() {
        --end_;
        data_allocator::destroy(end_);
    }

    iterator insert(const_iterator position, const value_type& val) {
        return emplace(position, val);
    }
    iterator insert(const_iterator position, size_type n, const value_type& val) {
        return fill_insert(const_cast<iterator>(position), n, val);
    }
    template <class InputIterator, typename = mystl::RequireInputIterator<InputIterator>>
    iterator insert(const_iterator position, InputIterator first, InputIterator last) {
        insert_dispatch(const_cast<iterator>(position), first, last, std::false_type());
        return const_cast<iterator>(position);
    }
    iterator insert(const_iterator position, value_type&& val) {
        return emplace(position, mystl::move(val));
    }
    iterator insert(const_iterator position, std::initializer_list<value_type> il) {
        insert_dispatch(const_cast<iterator>(position), il.begin(), il.end(), std::false_type());
        return const_cast<iterator>(position);
    }
    iterator erase(iterator position) {
        return erase(position, position + 1);
    }

    iterator erase(iterator first, iterator last) {
        mystl::move(last, end_, first); // FIXME
        auto new_end = end_ - (last - first);
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
    iterator emplace(const_iterator position, Args&&... args) {
        return emplace_aux(const_cast<iterator>(position), mystl::forward<Args>(args)...);
    }

    template <class... Args>
    void emplace_back(Args&&... args) {
        emplace_aux(end_, mystl::forward<Args>(args)...);
    }

    /*
     * Allocator
     */
    allocator_type get_allocator() const noexcept {
        return data_allocator();
    }

private:
    void fill_initialize(size_type n, const_reference val) {
        begin_ = data_allocator::allocate(n);
        mystl::uninitialized_fill_n(begin_, n, val);
        end_ = begin_ + n;
        capacity_ = end_;
    }

    template <class InputIterator>
    void range_initialize(InputIterator first, InputIterator last) {
        size_type n = size_type(last - first);
        begin_ = data_allocator::allocate(n);
        mystl::uninitialized_copy(first, last, begin_);
        end_ = begin_ + n;
        capacity_ = end_;
    }

    template <class InputIterator>
    void destructor(InputIterator first, InputIterator last, size_type n) {
        data_allocator::destroy(first, last);
        data_allocator::deallocate(first, n);
    }

    size_type check_len(size_type n) const {
        if (max_size() - size() < n)
            throw; // TODO

        const size_type len = size() + mystl::max(size(), n);
        return (len < size() || len > max_size()) ? max_size() : len;
    }

    template <class Integer>
    void assign_dispatch(Integer n, Integer val, std::true_type) { // FIXME: not used
        fill_assign(n, val);
    }

    template <class InputIterator>
    void assign_dispatch(InputIterator first, InputIterator last, std::false_type) {
        range_assign(first, last, mystl::iterator_category(first));
    }

    iterator fill_assign(size_type n, const value_type& val) {
        if (n > capacity()) {
            vector tmp(n, val);
            swap(tmp);
        } else if (n > size()) {
            mystl::fill(begin_, end_, val);
            mystl::uninitialized_fill_n(end_, begin_ + n, val);
        } else {
            erase(begin_ + n, end_);
        }
        return begin_ + n;
    }

    template <class InputIterator>
    void range_assign(InputIterator first, InputIterator last, mystl::input_iterator_tag) {
        auto cur = begin_;
        for (; first != last && cur != end_; ++cur, ++first) {
            *cur = *first;
        }
        if (first == last) {
            erase(cur, end_);
        } else {
            insert(end_, first, last);
        }
    }

    template <class ForwardIterator>
    void range_assign(ForwardIterator first, ForwardIterator last, mystl::forward_iterator_tag) {
        const size_type len = mystl::distance(first, last);
        if (len > capacity()) {
            vector tmp(first, last);
            swap(tmp);
        } else if (len > size()) {
            auto mid = first;
            mystl::advance(mid, size());
            mystl::copy(first, mid, begin_);
            auto new_end = mystl::uninitialized_copy(mid, last, end_);
            end_ = new_end;
        } else {
            erase(mystl::copy(first, last, begin_), end_);
        }
    }

    template <typename Integer>
    void insert_dispatch(iterator pos, Integer n, Integer val, std::true_type) {
        fill_insert(pos, n, val);
    }

    template <typename InputIterator>
    void insert_dispatch(iterator pos, InputIterator first, InputIterator last, std::false_type) {
        range_insert(pos, first, last, mystl::iterator_category(first));
    }

    template <typename InputIterator>
    void range_insert(iterator pos, InputIterator first, InputIterator last, mystl::input_iterator_tag) {
        if (pos == end_) {
            for (; first != last; ++first) {
                insert(end_, *first);
            }
        } else if (first != last) {
            vector tmp(first, last);
            insert(pos, tmp.begin_, tmp.end_);
        }
    }

    template <typename _ForwardIterator>
    void range_insert(iterator pos, _ForwardIterator first, _ForwardIterator last, mystl::forward_iterator_tag) {
        if (first == last) { return; }
        const size_type n = std::distance(first, last);
        if (static_cast<size_type>(capacity_ - end_) >= n) { // 备用空间足够插入
            const auto elems_after = end_ - pos;
            if (elems_after > n) {
                mystl::uninitialized_move(end_ - n, end_, end_); // 移动n个元素到备用空间
                mystl::move_backward(pos, end_ - n, end_);       // pos后的剩余部分从后往前开始移动,防止内存重叠
                mystl::uninitialized_copy(first, last, pos);     // 填充元素
            } else {
                mystl::uninitialized_copy(first + elems_after, last, end_);     // 不足的部分填充到备用空间
                mystl::uninitialized_move(pos, end_, end_ + (n - elems_after)); // pos后的元素移动到填充后的空备空间尾
                mystl::uninitialized_copy(first, first + elems_after, pos);     // 填充移动后的空间
            }
            end_ += n;
        } else { // 空间不足需要申请新内存
            const size_type len = check_len(n);
            const size_type elems_before = pos - begin_;
            iterator new_begin = data_allocator::allocate(len);
            iterator new_end = new_begin;
            try {
                new_end = mystl::uninitialized_move(begin_, pos, new_begin);                  // 插入前的元素移动到新的空间
                new_end = mystl::uninitialized_copy(first, last, new_begin + elems_before);   // 再填充要插入的元素
                new_end = mystl::uninitialized_move(pos, end_, new_begin + elems_before + n); // 最后将插入后的元素移动过来
            } catch (...) {
                destructor(new_begin, new_end, len);
                throw;
            }
            destructor(begin_, end_, capacity());

            begin_ = new_begin;
            end_ = new_end;
            capacity_ = new_begin + len;
        }
        return;
    }

    iterator fill_insert(iterator pos, size_type n, const value_type& val) {
        if (n == 0) { return pos; }
        if (static_cast<size_type>(capacity_ - end_) >= n) { // 备用空间足够插入
            const auto elems_after = end_ - pos;
            if (elems_after > n) {
                mystl::uninitialized_move(end_ - n, end_, end_); // 移动n个元素到备用空间
                mystl::move_backward(pos, end_ - n, end_);       // pos后的剩余部分从后往前开始移动,防止内存重叠
                mystl::uninitialized_fill_n(pos, n, val);        // 填充元素
            } else {
                mystl::uninitialized_fill_n(end_, n - elems_after, val);        // 不足的部分填充到备用空间
                mystl::uninitialized_move(pos, end_, end_ + (n - elems_after)); // pos后的元素移动到填充后的空备空间尾
                mystl::uninitialized_fill_n(pos, elems_after, val);             // 填充移动后的空间
            }
            end_ += n;
        } else { // 空间不足需要申请新内存
            const size_type len = check_len(n);
            const size_type elems_before = pos - begin_;
            iterator new_begin = data_allocator::allocate(len);
            iterator new_end = new_begin;
            try {
                new_end = mystl::uninitialized_move(begin_, pos, new_begin);                  // 插入前的元素移动到新的空间
                new_end = mystl::uninitialized_fill_n(new_begin + elems_before, n, val);      // 再填充要插入的元素
                new_end = mystl::uninitialized_move(pos, end_, new_begin + elems_before + n); // 最后将插入后的元素移动过来
            } catch (...) {
                destructor(new_begin, new_end, len);
                throw;
            }
            destructor(begin_, end_, capacity());

            begin_ = new_begin;
            end_ = new_end;
            capacity_ = new_begin + len;
        }
        return pos + n;
    }

    void realloc_insert(iterator pos, const_reference val) {
        const size_type len = check_len(1);
        iterator new_begin = data_allocator::allocate(len);
        iterator new_end = new_begin;
        try {
            new_end = mystl::uninitialized_copy(begin_, pos, new_begin);
            data_allocator::construct(new_end, val);
            ++new_end;
            new_end = mystl::uninitialized_copy(pos, end_, new_end);
        } catch (...) {
            destructor(new_begin, new_end, len);
            throw;
        }
        destructor(begin_, end_, capacity()); // 移动后的内存可以正常执行析构

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
            destructor(new_begin, new_end, len);
            throw;
        }
        destructor(begin_, end_, capacity());

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

template <class T, class Alloc>
bool operator==(const vector<T, Alloc>& lhs, const vector<T, Alloc>& rhs) {
    if (lhs.size() != rhs.size()) { return false; }
    return mystl::equal(lhs.begin(), lhs.end(), rhs.begin());
}

template <class T, class Alloc>
bool operator!=(const vector<T, Alloc>& lhs, const vector<T, Alloc>& rhs) {
    return !(lhs == rhs);
}

template <class T, class Alloc>
bool operator<(const vector<T, Alloc>& lhs, const vector<T, Alloc>& rhs);
template <class T, class Alloc>
bool operator<=(const vector<T, Alloc>& lhs, const vector<T, Alloc>& rhs);
template <class T, class Alloc>
bool operator>(const vector<T, Alloc>& lhs, const vector<T, Alloc>& rhs);
template <class T, class Alloc>
bool operator>=(const vector<T, Alloc>& lhs, const vector<T, Alloc>& rhs);

template <class T, class Alloc>
void swap(vector<T, Alloc>& x, vector<T, Alloc>& y);

// bool specialization
template <class Alloc>
class vector<bool, Alloc> {
};
} // namespace mystl

#endif // MYSTL_VECTOE_H_
