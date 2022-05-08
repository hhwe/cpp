#ifndef MYSTL_DEQUE_H_
#define MYSTL_DEQUE_H_

#include <cstddef>

#include "allocator.h"
#include "iterator.h"
#include "construct.h"
#include "uninitialized.h"
#include "functional.h"
#include "functexcept.h"

namespace mystl {
constexpr std::size_t DEQUE_BUFFER_SIZE = 512U;
constexpr std::size_t DEQUE_INIT_MAP_SIZE = 8U;
std::size_t deque_buf_size(std::size_t size) {
    return (size < DEQUE_BUFFER_SIZE) ? std::size_t(DEQUE_BUFFER_SIZE / size) : std::size_t(1);
}

template <typename T, typename Ref, typename Ptr>
class deque_iterator {
public:
    using self = deque_iterator<T, Ref, Ptr>;

    using iterator_category = mystl::random_access_iterator_tag;
    using value_type = T;
    using pointer = Ptr;
    using reference = Ref;
    using size_type = std::size_t;
    using difference_type = std::ptrdiff_t;

    using node_pointer = value_type*;
    using map_pointer = node_pointer*;

public:
    deque_iterator() :
        cur_(), first_(), last_(), node_() {
    }
    explicit deque_iterator(node_pointer x, map_pointer y) :
        cur_(x), first_(*y), last_(*y + buffer_size()), node_(y) {
    }

    reference operator*() const {
        return *cur_;
    }

    pointer operator->() const {
        return &(operator*());
    }

    self& operator++() {
        ++cur_;
        if (cur_ == last_) {
            set_node(node_ + 1);
            cur_ = first_;
        }
        return *this;
    }

    self operator++(int) {
        self tmp = *this;
        ++*this;
        return tmp;
    }

    self& operator--() {
        if (cur_ == first_) {
            set_node(node_ - 1);
            cur_ = last_;
        }
        --cur_;
        return *this;
    }

    self operator--(int) {
        self tmp = *this;
        node_ = node_->prev_;
        return tmp;
    }

    self& operator+=(difference_type n) {
        const difference_type offset = n + (cur_ - first_);
        if (offset >= 0 && offset < difference_type(buffer_size())) {
            cur_ += n;
        } else {
            const difference_type node_offset =
                offset > 0 ? offset / difference_type(buffer_size()) :
                             -difference_type((-offset - 1) / buffer_size()) - 1;
            set_node(node_ + node_offset);
            cur_ = first_ + (offset - node_offset * (difference_type(buffer_size())));
        }
        return *this;
    }

    self operator+(difference_type n) const noexcept {
        self tmp = *this;
        return tmp += n;
    }

    self& operator-=(difference_type n) {
        return operator+=(-n);
    }

    self operator-(difference_type n) const noexcept {
        return operator+(-n);
    }

    self operator[](difference_type n) const noexcept {
        return *(*this + n);
    }

    bool operator==(const self rhs) const {
        return cur_ == rhs.cur_;
    }

    bool operator!=(const self rhs) const {
        return !(operator==(rhs));
    }

private:
    static constexpr std::size_t buffer_size() {
        return deque_buf_size(sizeof(T));
    }

    void set_node(map_pointer new_node) noexcept {
        node_ = new_node;
        first_ = *new_node;
        last_ = first_ + difference_type(buffer_size());
    }

private:
    node_pointer cur_;
    node_pointer first_;
    node_pointer last_;
    map_pointer node_;
};

template <typename T, typename Alloc = mystl::allocator<T>>
class deque {
public: // member types
    using allocator_type = Alloc;

    using value_type = typename allocator_type::value_type;
    using pointer = typename allocator_type::pointer;
    using const_pointer = typename allocator_type::const_pointer;
    using reference = typename allocator_type::reference;
    using const_reference = typename allocator_type::const_reference;
    using size_type = typename allocator_type::size_type;
    using difference_type = typename allocator_type::difference_type;

    using iterator = deque_iterator<T, T&, T*>;
    using const_iterator = deque_iterator<T, const T&, const T*>;
    using reverse_iterator = mystl::reverse_iterator<iterator>;
    using const_reverse_iterator = mystl::reverse_iterator<const_iterator>;

    using data_allocator = Alloc;
    using node_pointer = typename iterator::node_pointer;
    using map_pointer = typename iterator::map_pointer;
    using map_allocator = typename Alloc::template rebind<node_pointer>::other;

public: // member functions
    /*
     * @brief Constructor and Destructor
     */
    // default (1)
    explicit deque(const allocator_type& alloc = allocator_type()) {
        fill_initialize(0, value_type());
    }
    // fill (2)
    explicit deque(size_type n) {
        fill_initialize(n, value_type());
    }
    deque(size_type n, const value_type& val, const allocator_type& alloc = allocator_type()) {
        fill_initialize(n, val);
    }
    // range (3)
    template <class InputIterator, typename = mystl::RequireInputIterator<InputIterator>>
    deque(InputIterator first, InputIterator last, const allocator_type& alloc = allocator_type()) {
        range_initialize(first, last);
    }
    // copy (4)
    deque(const deque& x) {
        range_initialize(x.begin(), x.end());
    }
    // deque(const deque& x, const allocator_type& alloc);
    // move (5)
    deque(deque&& x) {
        deque(mystl::move(x));
    }
    // deque(deque&& x, const allocator_type& alloc);
    // initializer deque (6)
    deque(std::initializer_list<value_type> il, const allocator_type& alloc = allocator_type()) {
        range_initialize(il.begin(), il.end());
    }

    ~deque() {
        if (map_ != nullptr) {
            clear();
            map_allocator::deallocate(map_, map_size_);
            map_ = nullptr;
            map_size_ = 0U;
        }
    }

    // copy (1)
    deque& operator=(const deque& x);
    // move (2)
    deque& operator=(deque&& x);
    // initializer deque (3)
    deque& operator=(std::initializer_list<value_type> il);

    /*
     * @brief Iterators
     */
    iterator begin() noexcept {
        return start_;
    }
    const_iterator begin() const noexcept {
        return const_iterator(start_);
    }

    iterator end() noexcept {
        return finish_;
    }
    const_iterator end() const noexcept {
        return const_iterator(finish_);
    }

    reverse_iterator rbegin() noexcept {
        return reverse_iterator(end());
    }
    const_reverse_iterator rbegin() const noexcept {
        return const_reverse_iterator(end());
    }

    reverse_iterator rend() noexcept {
        return reverse_iterator(begin());
    }
    const_reverse_iterator rend() const noexcept {
        return const_reverse_iterator(begin());
    }

    const_iterator cbegin() const noexcept {
        return const_iterator(begin());
    }

    const_iterator cend() const noexcept {
        return const_iterator(end());
    }

    const_reverse_iterator crbegin() const noexcept {
        return const_reverse_iterator(rbegin());
    }

    const_reverse_iterator crend() const noexcept {
        return const_reverse_iterator(rend());
    }

    /*
     * @brief Capacity
     */

    size_type size() const noexcept {
        return finish_ - start_;
    }

    size_type max_size() const noexcept {
        return static_cast<size_type>(-1);
    }

    void resize(size_type n) {
        resize(n, value_type());
    }
    void resize(size_type n, const value_type& val) {
        if (n > size()) {
            insert(end(), n - size(), val);
        } else {
            erase(begin() + n, end());
        }
    }

    bool empty() const noexcept {
        return finish_ == start_;
    }

    /*
     * @brief Element access
     */
    reference operator[](size_type n) {
        return *(start_[n]);
    }
    const_reference operator[](size_type n) const;

    reference at(size_type n) {
        THROW_OUT_OF_RANGE_IF(n >= size(), "deque.at");
        return (*this)[n];
    }
    const_reference at(size_type n) const;

    reference front() {
        return *begin();
    }
    const_reference front() const;

    reference back() {
        iterator tmp = end();
        --tmp;
        return *tmp;
    }
    const_reference back() const;

    /*
     * @brief Modifiers
     */
    // range (1)
    template <class InputIterator, typename = mystl::RequireInputIterator<InputIterator>>
    void assign(InputIterator first, InputIterator last) {
        range_assign(first, last);
    }
    // fill (2)
    void assign(size_type n, const value_type& val) {
        fill_assign(n, val);
    }
    // initializer deque (3)
    void assign(std::initializer_list<value_type> il) {
        range_assign(il.begin(), il.end());
    }

    template <class... Args>
    void emplace_front(Args&&... args) {
        if (start_.cur_ != start_.first_) {
            data_allocator::construct(start_.cur_, mystl::forward<Args>(args)...);
            --start_.cur_;
        } else {
            push_front_aux(mystl::forward<Args>(args)...);
        }
    }

    void push_front(const value_type& val) {
        emplace_front(val);
    }
    void push_front(value_type&& val) {
        emplace_front(std::move(val));
    }

    void pop_front() {
        if (start_.cur_ != start_.last_ - 1) {
            data_allocator::destroy(start_.cur_);
            ++start_.cur_;
        } else {
        }
    }

    template <class... Args>
    void emplace_back(Args&&... args) {
        if (finish_.cur_ != finish_.last_ - 1) {
            data_allocator::construct(finish_.cur_, mystl::forward<Args>(args)...);
            ++finish_.cur_;
        } else {
            push_back_aux(mystl::forward<Args>(args)...);
        }
    }

    void push_back(const value_type& val) {
        emplace_back(val);
    }
    void push_back(value_type&& val) {
        emplace_back(std::move(val));
    }

    void pop_back() {
        // erase(iterator(node_->prev_));
    }

    template <class... Args>
    iterator emplace(iterator position, Args&&... args) {
        if (position.cur_ == start_.cur_) {
            emplace_front(mystl::forward<Args>(args)...);
            return start_;
        } else if (position.cur_ == finish_.cur_) {
            emplace_back(mystl::forward<Args>(args)...);
            return finish_;
        }
        return insert_aux(position, mystl::forward<Args>(args)...);
    }

    // single element (1)
    iterator insert(iterator position, const value_type& val) {
        return emplace(position, val);
    }
    // fill (2)
    iterator insert(iterator position, size_type n, const value_type& val) {
        deque tmp(n, val);
        if (tmp.empty()) { return position; }
        iterator it = tmp.begin();
        splice(position, tmp);
        return it;
    }
    // range (3)
    template <class InputIterator, typename = mystl::RequireInputIterator<InputIterator>>
    iterator insert(iterator position, InputIterator first, InputIterator last) {
        deque tmp(first, last);
        if (tmp.empty()) { return position; }
        iterator it = tmp.begin();
        splice(position, tmp);
        return it;
    }
    // move (4)
    iterator insert(iterator position, value_type&& val) {
        return emplace(position, mystl::move(val));
    }
    // initializer deque (5)
    iterator insert(iterator position, std::initializer_list<value_type> il) {
        return insert(position, il.begin(), il.end());
    }

    iterator erase(iterator position) {
        iterator next = position;
        ++next;
        const difference_type index = position - begin();
        if (position != begin()) {
        }
        return next;
    }
    iterator erase(iterator first, iterator last) {
        while (first != last) {
            first = erase(first);
        }
        return last;
    }

    void swap(deque& x) {
        mystl::swap(map_, x.map_);
        mystl::swap(map_size_, x.map_size_);
        mystl::swap(start_, x.start_);
        mystl::swap(finish_, x.finish_);
    }

    void clear() noexcept {
        mystl::destroy(start_, finish_);
        delete_nodes(start_.node_, finish_.node_);
        finish_ = start_;
    }

    allocator_type get_allocator() const noexcept {
        return allocator_type();
    }

private:
    static constexpr std::size_t buffer_size() {
        return deque_buf_size(sizeof(T));
    }

    void delete_node(node_pointer node) {
        return data_allocator::deallocate(node, buffer_size());
    }

    void delete_nodes(map_pointer nstart, map_pointer nfinish) {
        map_pointer cur;
        for (cur = nstart; cur < nfinish; ++cur) {
            delete_node(*cur);
        }
    }

    node_pointer create_node() {
        return data_allocator::allocate(buffer_size());
    }

    void create_nodes(map_pointer nstart, map_pointer nfinish) {
        map_pointer cur;
        try {
            for (cur = nstart; cur < nfinish; ++cur) {
                *cur = create_node();
            }
        } catch (...) {
            delete_nodes(nstart, cur);
            throw;
        }
    }

    void init_map(size_type num_elemens) {
        const size_type num_nodes = (num_elemens / buffer_size()) + 1;
        map_size_ = mystl::max(DEQUE_INIT_MAP_SIZE, num_nodes + 2);
        map_ = map_allocator::allocate(map_size_);

        // 将头尾指针指向中间位置,方便前后扩展
        map_pointer nstart = map_ + (map_size_ - num_nodes) / 2;
        map_pointer nfinish = nstart + num_nodes;

        try {
            create_nodes(nstart, nfinish);
        } catch (...) {
            map_allocator::deallocate(map_, map_size_);
            map_ = map_pointer();
            map_size_ = 0;
            throw;
        }

        start_.set_node(nstart);
        finish_.set_node(nfinish - 1);
        start_.cur_ = start_.first_;
        finish_.cur_ = finish_.first_ + num_elemens % buffer_size();
    }

    void reserve_map_at_front(size_type nodes_to_add = 1) {
        if (nodes_to_add > start_.node_ - map_) { // start.first指向最前一个node的头
            reallocate_map(nodes_to_add, true);
        }
    }

    void reserve_map_at_back(size_type nodes_to_add = 1) {
        if (nodes_to_add + 1 > map_size_ - (finish_.node_ - map_)) { // finish.last指向最后一个node的尾
            reallocate_map(nodes_to_add, false);
        }
    }

    void reallocate_map(size_type nodes_to_add, bool add_at_front) {
        const size_type old_num_nodes = finish_.node_ - start_.node_ + 1;
        const size_type new_num_nodes = old_num_nodes + nodes_to_add;

        map_pointer new_nstart;
        if (map_size_ > 2 * new_num_nodes) {
            new_nstart = map_ + (map_size_ - new_num_nodes) / 2 + (add_at_front ? nodes_to_add : 0);
            if (new_nstart < start_.node_) {
                mystl::copy(start_.node_, finish_.node_ + 1, new_nstart);
            } else {
                mystl::copy_backward(start_.node_, finish_.node_ + 1, new_nstart + old_num_nodes);)
            }
        } else {
            size_type new_map_size = map_size_ + mystl::max(map_size_, nodes_to_add) + 2;
            map_pointer new_map = map_allocator::allocate(new_map_size);
            new_nstart = new_map + (new_map_size - new_num_nodes) / 2 + (add_at_front ? nodes_to_add : 0);
            mystl::copy(start_.node_, finish_.node_ + 1, new_nstart);
            map_allocator::deallocate(map_, map_size_);
            map_ = new_map;
            map_size_ = new_map_size;
        }
        start_.set_node(new_nstart);
        finish_.set_node(new_nstart + old_num_nodes - 1);
    }

    void fill_initialize(size_type n, const_reference val) {
        init_map(n);
        if (n == 0) { return; }
        map_pointer cur;
        try {
            for (cur = start_.node_; cur < finish_.node_; ++cur) {
                mystl::uninitialized_fill(*cur, *cur + buffer_size(), val);
            }
            mystl::uninitialized_fill(finish_.first_, finish_.cur_, val);
        } catch (...) {
            mystl::destroy(start_, iterator(*cur, cur));
            throw;
        }
    }

    template <class InputIterator>
    void range_initialize_aux(InputIterator first, InputIterator last, mystl::input_iterator_tag) {
        init_map(0);
        try {
            for (; first != last; ++first) {
                emplace_back(*first);
            }
        } catch (...) {
            clear();
            throw;
        }
    }

    template <class ForwardIterator>
    void range_initialize_aux(ForwardIterator first, ForwardIterator last, mystl::forward_iterator_tag) {
        const size_type n = mystl::distance(first, last);
        init_map(n);
        map_pointer cur;
        try {
            for (cur = start_.node_; cur < finish_.node_; ++cur) {
                ForwardIterator mid = first;
                mystl::advance(mid, buffer_size());
                mystl::uninitialized_copy(first, mid, *cur);
                first = mid;
            }
            mystl::uninitialized_copy(first, last, finish_.first_);
        } catch (...) {
            mystl::destroy(start_, iterator(*cur, cur));
            throw;
        }
    }

    template <class InputIterator>
    void range_initialize(InputIterator first, InputIterator last) {
        range_initialize_aux(first, last, iterator_category(first));
    }

    void fill_assign(size_type n, const value_type& val) {
        auto cur = begin();
        for (; cur != end() && n > 0; ++cur, --n) {
            *cur = *val;
        }
        if (n > 0) {
            insert(end(), n, val);
        } else {
            erase(cur, end());
        }
    }

    template <class InputIterator>
    void range_assign(InputIterator first, InputIterator last) {
        auto cur = begin();
        for (; cur != end() && first != last; ++cur, ++first) {
            *cur = *first;
        }
        if (first != last) {
            insert(end(), first, last);
        } else {
            erase(cur, end());
        }
    }

    template <class... Args>
    iterator insert_aux(iterator pos, Args&&... args) {
        value_type x_copy(mystl::forward<Args>(args)...);
        difference_type index = pos - start_;
        if (index < size() / 2) { // 前半段
            push_front(mystl::move(front()));
            iterator front1 = start_;
            ++front1;
            iterator front2 = front1;
            ++front2;
            pos = start_ + index;
            iterator pos1 = pos;
            ++pos1;
            mystl::move(front2, pos1, front1);
        } else {
            push_back(mystl::move(back()));
            iterator back1 = finish_;
            --back1;
            iterator back2 = back1;
            --back2;
            pos = start_ + index;
            mystl::move(pos, back2, back1);
        }
        *pos = mystl::move(x_copy);
        return pos;
    }

    template <class... Args>
    void push_front_aux(Args&&... args) {
        THROW_OUT_OF_RANGE_IF(size() == max_size(), "out of range");
        reserve_map_at_front(); // 符合条件重换map
        *(start_.node_ - 1) = create_node();
        try {
            data_allocator::construct(start_.cur_, mystl::forward<Args>(args)...);
            start_.set_node(start_.node_ - 1);
            start_.cur_ = start_.last_ - 1;
        } catch (...) {
            delete_node(*(start_.node_ - 1));
            throw;
        }
    }

    template <class... Args>
    void push_back_aux(Args&&... args) {
        THROW_OUT_OF_RANGE_IF(size() == max_size(), "out of range");
        reserve_map_at_back(); // 符合条件重换map
        *(finish_.node_ + 1) = create_node();
        try {
            data_allocator::construct(finish_.cur_, mystl::forward<Args>(args)...);
            finish_.set_node(finish_.node_ + 1);
            finish_.cur_ = finish_.first_;
        } catch (...) {
            delete_node(*(finish_.node_ + 1));
            throw;
        }
    }

private:
    map_pointer map_;
    size_type map_size_;
    iterator start_;
    iterator finish_;
};

// (1)
template <class T, class Alloc>
bool operator==(const deque<T, Alloc>& lhs, const deque<T, Alloc>& rhs) {
    if (lhs.size() != rhs.size()) { return false; }
    return mystl::equal(lhs.begin(), lhs.end(), rhs.begin());
}
// (2)
template <class T, class Alloc>
bool operator!=(const deque<T, Alloc>& lhs, const deque<T, Alloc>& rhs) {
    return !(lhs == rhs);
}
// (3)
template <class T, class Alloc>
bool operator<(const deque<T, Alloc>& lhs, const deque<T, Alloc>& rhs);
// (4)
template <class T, class Alloc>
bool operator<=(const deque<T, Alloc>& lhs, const deque<T, Alloc>& rhs);
// (5)
template <class T, class Alloc>
bool operator>(const deque<T, Alloc>& lhs, const deque<T, Alloc>& rhs);
// (6)
template <class T, class Alloc>
bool operator>=(const deque<T, Alloc>& lhs, const deque<T, Alloc>& rhs);

template <class T, class Alloc>
void swap(deque<T, Alloc>& x, deque<T, Alloc>& y) {
    x.swap(y);
}

} // namespace mystl

#endif // MYSTL_DEQUE_H_
