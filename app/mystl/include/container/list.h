#ifndef MYSTL_LIST_H_
#define MYSTL_LIST_H_

#include "allocator.h"
#include "iterator.h"
#include "construct.h"
#include "uninitialized.h"

namespace mystl {

// struct list_node_base {
//     list_node_base* next_;
//     list_node_base* prev_;
// };

template <typename T>
struct list_node {
    list_node* next_;
    list_node* prev_;
    T data_;
};

// class list_node_header : public list_node_base {
// public:
//     std::size_t size_;

//     list_node_header() noexcept {
//         init();
//     }

//     list_node_header(list_node_header&& x) noexcept :
//         list_node_base{x.next_, x.prev_}, size_(x.size_) {
//         if (x.base()->next_ = x.base()) {
//             next_ = prev_ = this;
//         } else {
//             next_->prev_ = prev_->next_ = this->base();
//             x.init();
//         }
//     }

//     void move_node(list_node_header&& x) {
//         list_node_base* const xnode = x.base();
//         if (xnode->next_ == xnode)
//             init();
//         else {
//             list_node_base* const node = this->base();
//             node->next_ = xnode->next_;
//             node->prev_ = xnode->prev_;
//             node->next_->prev_ = node->prev_->next_ = node;
//             size_ = x.size_;
//             x.init();
//         }
//     }

// private:
//     void init() noexcept {
//         next_ = this;
//         prev_ = this;
//         size_ = 0;
//     }

//     list_node_base* base() {
//         return this;
//     }
// };

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
        return (static_cast<node_pointer>(node_))->data_;
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

private:
    node_pointer node_;
};

// template <typename T, typename Alloc>
// class list_base {
// public: // member types
//     using allocator_type = Alloc;
//     using node = list_node<T>;
//     using node_pointer = node*;
//     using node_allocator = typename Alloc::template rebind<node>::other;
//     using node_base = list_node_base<T>;
//     using node_base_pointer = node_base*;

// public:
//     list_base(const allocator_type&) {
//         node_ = get_node();
//         node_->next_ = node_;
//         node_->prev_ = node_;
//     }
//     ~list_base() {
//         clear();
//         put_node(node_);
//     }

// protected:
//     // 配置一个节点
//     node_pointer get_node() {
//         return node_allocator::allocate(1);
//     }

//     // 释放一个节点
//     void put_node(node_pointer p) {
//         node_allocator::deallocate(p, 1);
//     }

//     void clear() {
//         node_base_pointer cur = head->next_;
//         while (cur != head.base()) {
//             node_pointer tmp = cur;
//             cur = tmp->next_;
//             cur = static_cast<node_pointer>(tmp->next_);
//             mystl::destroy(&cur->data_);
//             put_node(tmp);
//         }
//         head->next_ = head;
//         head->prev_ = head;
//     }

// protected:
//     list_node_header head;
// };

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
        node_ = get_node();
        node_->next_ = node_;
        node_->prev_ = node_;
        size_ = 0U;
    }
    // fill (2)
    explicit list(size_type n);
    list(size_type n, const value_type& val,
         const allocator_type& alloc = allocator_type());
    // range (3)
    template <class InputIterator>
    list(InputIterator first, InputIterator last,
         const allocator_type& alloc = allocator_type());
    // copy (4)
    list(const list& x);
    list(const list& x, const allocator_type& alloc);
    // move (5)
    list(list&& x);
    list(list&& x, const allocator_type& alloc);
    // initializer list (6)
    list(std::initializer_list<value_type> il,
         const allocator_type& alloc = allocator_type());

    ~list() {
        if (node_ != nullptr) {
            clear();
            put_node(node_);
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
        return node_->next_ == node_;
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
        return *(end() - 1);
    }
    const_reference back() const {
        return *(end() - 1);
    }

    /*
     * @brief Modifiers
     */
    // range (1)
    template <class InputIterator>
    void assign(InputIterator first, InputIterator last);
    // fill (2)
    void assign(size_type n, const value_type& val) {
        fill_assign(n, val);
    }
    // initializer list (3)
    void assign(initializer_list<value_type> il);

    template <class... Args>
    void emplace_front(Args&&... args);

    void push_front(const value_type& val);
    void push_front(value_type&& val);

    void pop_front();

    template <class... Args>
    void emplace_back(Args&&... args);

    void push_back(const value_type& val);
    void push_back(value_type&& val);

    void pop_back();

    template <class... Args>
    iterator emplace(const_iterator position, Args&&... args);

    // single element (1)
    iterator insert(const_iterator position, const value_type& val);
    // fill (2)
    iterator insert(const_iterator position, size_type n, const value_type& val) {
    }
    // range (3)
    template <class InputIterator>
    iterator insert(const_iterator position, InputIterator first, InputIterator last);
    // move (4)
    iterator insert(const_iterator position, value_type&& val);
    // initializer list (5)
    iterator insert(const_iterator position, initializer_list<value_type> il);

    iterator erase(const_iterator position);
    iterator erase(const_iterator first, const_iterator last);
    void swap(list& x);
    void resize(size_type n);
    void resize(size_type n, const value_type& val);
    void clear() noexcept;

    /*
     * @brief Operations
     */
    // entire list (1)
    void splice(const_iterator position, list& x) {
        if (x.empty()) { return; }
        THROW_LENGTH_ERROR_IF(x.size() > max_size() - size(), "x's size too big");
    }
    void splice(const_iterator position, list&& x);
    // single element (2)
    void splice(const_iterator position, list& x, const_iterator i);
    void splice(const_iterator position, list&& x, const_iterator i);
    // element range (3)
    void splice(const_iterator position, list& x,
                const_iterator first, const_iterator last);
    void splice(const_iterator position, list&& x,
                const_iterator first, const_iterator last);
    void remove(const value_type& val);
    template <class Predicate>
    void remove_if(Predicate pred);
    //   (1)
    void unique();
    // (2)
    template <class BinaryPredicate>
    void unique(BinaryPredicate binary_pred);

    //   (1)
    void merge(list& x);
    void merge(list&& x);
    // (2)
    template <class Compare>
    void merge(list& x, Compare comp);
    template <class Compare>
    void merge(list&& x, Compare comp);

    // (1)
    void sort();
    // (2)
    template <class Compare>
    void sort(Compare comp);

    void reverse() noexcept;

    allocator_type get_allocator() const noexcept {
        return allocator_type();
    }

private:
    // 配置一个节点
    node_pointer get_node() {
        return node_allocator::allocate(1);
    }

    // 释放一个节点
    void put_node(node_pointer p) {
        node_allocator::deallocate(p, 1);
    }

    void clear() {
        node_pointer cur = static_cast<node_pointer>(node_->next_);
        while (cur != node_) {
            node_pointer tmp = cur;
            cur = static_cast<node_pointer>(cur->next_);
            mystl::destroy(&node_->data_);
            node_allocator::deallocate(tmp, 1);
        }
        node_->next_ = node_;
        node_->prev_ = node_;
    }

    template <typename... Args>
    node_pointer create_node(Aegs... args) {
        auto p = node_allocator::allocate(1);
        node_allocator::construct(p, mystl::forward<Args>(args)...);
        return p;
    }

    template <typename... Args>
    void insert_aux(iterator pos, Args&&... args) {
        node_pointer tmp = create_node(mystl::forward<Args>(args)...);
        tmp.hook(pos.node_);
        // tmp
    }

    void fill_assign(size_type n, const value_type& val) {
        auto i = begin();
        for (; i != end() && n > 0; ++i, --n) {
            *i = *val;
        }
        if (n > 0) {
            insert(end(), n, val);
        } else {
            erase(i, end());
        }
    }

private:
    node_pointer node_{nullptr};
    size_type size_{0U};
};

// (1)
template <class T, class Alloc>
bool operator==(const list<T, Alloc>& lhs, const list<T, Alloc>& rhs);
// (2)
template <class T, class Alloc>
bool operator!=(const list<T, Alloc>& lhs, const list<T, Alloc>& rhs);
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
void swap(list<T, Alloc>& x, list<T, Alloc>& y);

} // namespace mystl

#endif // MYSTL_LIST_H_
