#ifndef MYSTL_LIST_H_
#define MYSTL_LIST_H_

#include "allocator.h"
#include "iterator.h"
#include "construct.h"
#include "uninitialized.h"
#include "functional.h"

namespace mystl {

template <typename T>
struct list_node {
    list_node* next_{nullptr};
    list_node* prev_{nullptr};
    T data_;

    list_node() = default;
    list_node(const T& v) :
        data_(v) {
    }
    list_node(T&& v) :
        data_(mystl::move(v)) {
    }
    ~list_node() = default;

    static void swap(list_node& x, list_node& y) noexcept;

    // 将 [first, last) 内所有元素移动到 position 之前
    void transfer(list_node* const first, list_node* const last) noexcept {
        last->prev_->next_ = this;
        first->prev_->next_ = last;
        this->prev_->next_ = first;

        list_node* tmp = this->prev_;
        this->prev_ = last->prev_;
        last->prev_ = first->prev_;
        first->prev_ = tmp;
    }

    // 链表反转, 只移动头尾节点元素, 不调整指针
    void reverse() noexcept {
        list_node* next = this->next_;
        list_node* prev = this->prev_;
        while (next != prev && next->prev_ != prev) {
            mystl::swap(next->data_, prev->data_);
            next = next->next_;
            prev = prev->prev_;
        }
    }

    // 在链表的指定位置前插入本节点
    void hook(list_node* const position) noexcept {
        this->next_ = position;
        this->prev_ = position->prev_;
        position->prev_->next_ = this;
        position->prev_ = this;
    }

    // 在链表的指定位置删除本节点
    void unhook() noexcept {
        this->prev_->next_ = this->next_;
        this->next_->prev_ = this->prev_;
        this->next_ = nullptr;
        this->prev_ = nullptr;
    }

    // 把头尾节点从当前链表中删除
    void unlink(list_node* const first, list_node* const last) noexcept {
        first->prev_->next_ = last;
        last->prev_ = first->prev_;
    }

    void init() {
        prev_ = next_ = this;
    }
};

template <typename T, typename Ref, typename Ptr>
class list_iterator {
public:
    using self = list_iterator<T, Ref, Ptr>;
    using node = list_node<T>;
    using node_pointer = node*;

    using iterator_category = mystl::bidirectional_iterator_tag;
    using difference_type = std::ptrdiff_t;
    using value_type = T;
    using pointer = Ptr;
    using reference = Ref;

public:
    list_iterator() :
        node_() {
    }
    explicit list_iterator(node_pointer x) :
        node_(x) {
    }

    reference operator*() const {
        return node_->data_;
    }

    pointer operator->() const {
        return &(operator*());
    }

    self operator++() {
        node_ = node_->next_;
        return *this;
    }

    self operator++(int) {
        self tmp = *this;
        node_ = node_->next_;
        return tmp;
    }

    self operator--() {
        node_ = node_->prev_;
        return *this;
    }

    self operator--(int) {
        self tmp = *this;
        node_ = node_->prev_;
        return tmp;
    }

    bool operator==(const self rhs) const {
        return node_ == rhs.node_;
    }

    bool operator!=(const self rhs) const {
        return node_ != rhs.node_;
    }

public:
    node_pointer node_;
};

template <typename T, typename Alloc = mystl::allocator<T>>
class list {
public: // member types
    using allocator_type = Alloc;

    using value_type = typename allocator_type::value_type;
    using pointer = typename allocator_type::pointer;
    using const_pointer = typename allocator_type::const_pointer;
    using reference = typename allocator_type::reference;
    using const_reference = typename allocator_type::const_reference;
    using size_type = typename allocator_type::size_type;
    using difference_type = typename allocator_type::difference_type;

    using iterator = list_iterator<T, T&, T*>;
    using const_iterator = list_iterator<T, const T&, const T*>;
    using reverse_iterator = mystl::reverse_iterator<iterator>;
    using const_reverse_iterator = mystl::reverse_iterator<const_iterator>;

    using data_allocator = Alloc;
    using node = list_node<T>;
    using node_pointer = node*;
    using node_allocator = typename Alloc::template rebind<node>::other;

public: // member functions
    /*
     * @brief Constructor and Destructor
     */
    // default (1)
    explicit list(const allocator_type& alloc = allocator_type()) {
        init_node();
        size_ = 0U;
    }
    // fill (2)
    explicit list(size_type n) {
        fill_initialize(n, value_type());
    }
    list(size_type n, const value_type& val, const allocator_type& alloc = allocator_type()) {
        fill_initialize(n, val);
    }
    // range (3)
    template <class InputIterator, typename = mystl::RequireInputIterator<InputIterator>>
    list(InputIterator first, InputIterator last, const allocator_type& alloc = allocator_type()) {
        range_initialize(first, last);
    }
    // copy (4)
    list(const list& x) {
        range_initialize(x.begin(), x.end());
    }
    // list(const list& x, const allocator_type& alloc);
    // move (5)
    list(list&& x) {
        list(mystl::move(x));
    }
    // list(list&& x, const allocator_type& alloc);
    // initializer list (6)
    list(std::initializer_list<value_type> il, const allocator_type& alloc = allocator_type()) {
        range_initialize(il.begin(), il.end());
    }

    ~list() {
        if (node_ != nullptr) {
            clear();
            node_allocator::deallocate(node_);
            node_ = nullptr;
            size_ = 0U;
        }
    }

    // copy (1)
    list& operator=(const list& x);
    // move (2)
    list& operator=(list&& x);
    // initializer list (3)
    list& operator=(std::initializer_list<value_type> il);

    /*
     * @brief Iterators
     */
    iterator begin() noexcept {
        return iterator(node_->next_);
    }
    const_iterator begin() const noexcept {
        return const_iterator(node_->next_);
    }

    iterator end() noexcept {
        return iterator(node_);
    }
    const_iterator end() const noexcept {
        return const_iterator(node_);
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
    bool empty() const noexcept {
        return size_ == 0U;
    }

    size_type size() const noexcept {
        return size_;
    }

    size_type max_size() const noexcept {
        return static_cast<size_type>(-1);
    }

    /*
     * @brief Element access
     */
    reference front() {
        return *begin();
    }
    const_reference front() const {
        return *begin();
    }

    reference back() {
        iterator tmp = end();
        --tmp;
        return *tmp;
    }
    const_reference back() const {
        return back();
    }

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
    // initializer list (3)
    void assign(std::initializer_list<value_type> il) {
        range_assign(il.begin(), il.end());
    }

    template <class... Args>
    void emplace_front(Args&&... args) {
        emplace(begin(), mystl::forward<Args>(args)...);
    }

    void push_front(const value_type& val) {
        emplace_front(val);
    }
    void push_front(value_type&& val) {
        emplace_front(std::move(val));
    }

    void pop_front() {
        erase(begin());
    }

    template <class... Args>
    void emplace_back(Args&&... args) {
        emplace(end(), mystl::forward<Args>(args)...);
    }

    void push_back(const value_type& val) {
        emplace_back(val);
    }
    void push_back(value_type&& val) {
        emplace_back(std::move(val));
    }

    void pop_back() {
        erase(iterator(node_->prev_));
    }

    template <class... Args>
    iterator emplace(iterator position, Args&&... args) {
        node_pointer tmp = create_node(mystl::forward<Args>(args)...);
        tmp->hook(position.node_);
        ++size_;
        return iterator(tmp);
    }

    // single element (1)
    iterator insert(iterator position, const value_type& val) {
        return emplace(position, val);
    }
    // fill (2)
    iterator insert(iterator position, size_type n, const value_type& val) {
        list tmp(n, val);
        if (tmp.empty()) { return position; }
        iterator it = tmp.begin();
        splice(position, tmp);
        return it;
    }
    // range (3)
    template <class InputIterator, typename = mystl::RequireInputIterator<InputIterator>>
    iterator insert(iterator position, InputIterator first, InputIterator last) {
        list tmp(first, last);
        if (tmp.empty()) { return position; }
        iterator it = tmp.begin();
        splice(position, tmp);
        return it;
    }
    // move (4)
    iterator insert(iterator position, value_type&& val) {
        return emplace(position, mystl::move(val));
    }
    // initializer list (5)
    iterator insert(iterator position, std::initializer_list<value_type> il) {
        return insert(position, il.begin(), il.end());
    }

    iterator erase(iterator position) {
        iterator ret = iterator(position.node_->next_);
        --size_;
        position.node_->unhook();
        delete_node(position);
        return ret;
    }
    iterator erase(iterator first, iterator last) {
        while (first != last) {
            first = erase(first);
        }
        return last;
    }

    void swap(list& x) {
        mystl::swap(node_, x.node_);
        mystl::swap(size_, x.size_);
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

    void clear() noexcept {
        auto first = begin();
        auto last = end();
        while (first != last) {
            iterator tmp = first;
            ++first;
            delete_node(tmp);
        }
        node_->unlink(node_->next_, node_);
        size_ = 0;
    }

    /*
     * @brief Operations
     */
    // entire list (1)
    void splice(iterator position, list& x) {
        splice(position, mystl::move(x), x.begin(), x.end());
    }
    void splice(iterator position, list&& x) {
        splice(position, x, x.begin(), x.end());
    }
    // single element (2)
    void splice(iterator position, list& x, iterator i) {
        auto j = i++;
        splice(position, mystl::move(x), j, i);
    }
    void splice(iterator position, list&& x, iterator i) {
        auto j = i++;
        splice(position, x, j, i);
    }
    // element range (3)
    void splice(iterator position, list& x, iterator first, iterator last) {
        splice(position, mystl::move(x), first, last);
    }
    void splice(iterator position, list&& x, iterator first, iterator last) {
        auto len = mystl::distance(first, last);
        position.node_->transfer(first.node_, last.node_); // 节点拼接
        size_ += len;
        x.size_ -= len;
    }

    void remove(const value_type& val) {
        remove_if([&val](value_type& x) { return val == x; });
    }
    template <class Predicate>
    void remove_if(Predicate pred) {
        auto first = begin();
        auto last = end();
        while (first != last) {
            if (pred(*first)) {
                first = erase(first);
            } else {
                ++first;
            }
        }
    }

    //   (1)
    void unique() {
        unique([](value_type& x, value_type& y) { return x == y; });
    }
    // (2)
    template <class BinaryPredicate>
    void unique(BinaryPredicate binary_pred) {
        auto first = begin();
        auto prev = first++;
        auto last = end();
        while (first != last) {
            if (binary_pred(*first, *prev)) {
                first = erase(first);
            } else {
                prev = first++;
            }
        }
    }

    //   (1)
    void merge(list& x) {
        merge(mystl::move(x), mystl::less<value_type>());
    }
    void merge(list&& x) {
        merge(x, mystl::less<value_type>());
    }
    // (2)
    template <class Compare>
    void merge(list& x, Compare comp) {
        merge(mystl::move(x), comp);
    }
    template <class Compare>
    void merge(list&& x, Compare comp) {
        if (*this == x) { return; }
        auto first1 = begin();
        auto last1 = end();
        auto first2 = x.begin();
        auto last2 = x.end();
        while (first1 != last1 && first2 != last2) {
            if (comp(*first2, *first1)) {
                auto flag = first2;
                while ((++first2 != last2) && comp(*first2, *first1)) {}
                splice(first1, x, flag, first2);
            }
            ++first1;
        }
        if (first2 != last2) {
            splice(last1, x, first2, last2);
        }
    }

    // (1)
    void sort() {
        sort(mystl::less<value_type>());
    }
    // (2)
    template <class Compare>
    void sort(Compare comp) {
        if (size() <= 1) { return; } // 长度等于 0 或者 1 不进行处理
        list carry;
        list tmp[64]; // 按序号保存归并排序后list, 有序链表长度次幂级增长 1-2-4-8-...
        list* fill = tmp;
        list* counter;
        try {
            do {
                carry.splice(carry.begin(), *this, begin()); // 每次将第一个节点拼接到carry
                for (counter = tmp; counter != fill && !counter->empty(); ++counter) {
                    counter->merge(carry, comp); // 把carry按从小到大合并到counter(tmp[i](中, carry被置空
                    carry.swap(*counter);        // 已经排好序的counter放入到carry中,counter被置空
                }
                carry.swap(*counter); // carry中内容放入tmp[j](j是循环结束序列)中, carry被置空
                if (counter == fill) {
                    ++fill;
                }
            } while (!empty());
            // 拼接归并后的链表
            for (counter = tmp + 1; counter != fill; ++counter) {
                counter->merge(*(counter - 1), comp);
                swap(*(fill - 1));
            }
        } catch (...) {
            this->splice(this->end(), carry);
            for (int i = 0; i < sizeof(tmp) / sizeof(tmp[0]); ++i) {
                this->splice(this->end(), tmp[i]);
                throw;
            }
        }
    }

    void reverse() noexcept {
        node_->reverse();
    }

    allocator_type get_allocator() const noexcept {
        return allocator_type();
    }

private:
    void init_node() {
        node_ = node_allocator::allocate(1);
        node_->init();
    }

    void fill_initialize(size_type n, const_reference val) {
        init_node();
        for (; n; --n) {
            emplace_back(val);
        }
    }

    template <class InputIterator>
    void range_initialize(InputIterator first, InputIterator last) {
        init_node();
        for (; first != last; ++first) {
            emplace_back(*first);
        }
    }

    template <typename... Args>
    node_pointer create_node(Args... args) {
        auto p = node_allocator::allocate(1);
        node_allocator::construct(p, mystl::forward<Args>(args)...);
        return p;
    }

    void delete_node(iterator pos) {
        node_allocator::destroy(pos.node_);
        node_allocator::deallocate(pos.node_, 1);
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
        for (; cur != end() && first != end(); ++cur, ++first) {
            *cur = *first;
        }
        if (first != last) {
            insert(end(), first, last);
        } else {
            erase(cur, end());
        }
    }

private:
    node_pointer node_{nullptr};
    size_type size_{0U};
};

// (1)
template <class T, class Alloc>
bool operator==(const list<T, Alloc>& lhs, const list<T, Alloc>& rhs) {
    if (lhs.size() != rhs.size()) { return false; }
    return mystl::equal(lhs.begin(), lhs.end(), rhs.begin());
}
// (2)
template <class T, class Alloc>
bool operator!=(const list<T, Alloc>& lhs, const list<T, Alloc>& rhs) {
    return !(lhs == rhs);
}
// (3)
template <class T, class Alloc>
bool operator<(const list<T, Alloc>& lhs, const list<T, Alloc>& rhs);
// (4)
template <class T, class Alloc>
bool operator<=(const list<T, Alloc>& lhs, const list<T, Alloc>& rhs);
// (5)
template <class T, class Alloc>
bool operator>(const list<T, Alloc>& lhs, const list<T, Alloc>& rhs);
// (6)
template <class T, class Alloc>
bool operator>=(const list<T, Alloc>& lhs, const list<T, Alloc>& rhs);

template <class T, class Alloc>
void swap(list<T, Alloc>& x, list<T, Alloc>& y) {
    x.swap(y);
}

} // namespace mystl

#endif // MYSTL_LIST_H_
