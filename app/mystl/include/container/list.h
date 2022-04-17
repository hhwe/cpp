#ifndef MYSTL_LIST_H_
#define MYSTL_LIST_H_

#include "allocator.h"
#include "iterator.h"
#include "construct.h"
#include "uninitialized.h"

namespace mystl {

struct list_node_base {
    list_node_base* next_;
    list_node_base* prev_;
};

template <typename T>
struct list_node : public list_node_base {
    T data_;
};

template <typename T, typename Ref, typename Ptr>
class list_iterator {
public:
    using self = list_iterator<T, Ref, Ptr>;
    using node = list_node<T>;

    using iterator_category = mystl::bidirectional_iterator_tag;
    using difference_type = std::ptrdiff_t;
    using value_type = T;
    using pointer = Ptr;
    using reference = Ref;

public:
    list_iterator() :
        node_() {
    }
    explicit list_iterator(list_node_base* x) :
        node_(x) {
    }

    reference operator*() const {
        return (static_cast<node*>(node_))->data_;
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
    list_node_base* node_;
};

template <typename T, typename Alloc>
class list_base {
public: // member types
    using allocator_type = Alloc;
    using node = list_node<T>;

public:
    list_base(const allocator_type&) {
        node_ = get_node();
        node_->next_ = node_;
        node_->prev_ = node_;
    }
    ~list_base() {
        clear();
        put_node(node_);
    }

protected:
    using node_allocator = typename Alloc::template rebind<list_node<T>>::other;

    // 配置一个节点
    node* get_node() {
        return node_allocator::allocate(1);
    }

    // 释放一个节点
    void put_node(node* p) {
        node_allocator::deallocate(p, 1);
    }

    void clear() {
        node* cur = static_cast<node*>(node_->next_);
        while (cur != node_) {
            node* tmp = cur;
            cur = static_cast<node*>(cur->next_);
            mystl::destroy(&node_->data_);
            put_node(tmp);
        }
        node_->next_ = node_;
        node_->prev_ = node_;
    }

protected:
    node* node_;
};

template <typename T, typename Alloc = mystl::allocator<T>>
class list : protected list_base<T, Alloc> {
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
    using base = list_base<T, Alloc>;
    using base::node_;

public: // member functions
    /*
     * @brief Constructor and Destructor
     */
    // default (1)
    explicit list(const allocator_type& alloc = allocator_type()) :
        base(alloc) {
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

    // size_type size() const noexcept {
    //     return static_cast<size_type>(end_ - begin_);
    // }

    size_type max_size() const noexcept {
        return static_cast<size_type>(-1) / sizeof(value_type);
    }
};

} // namespace mystl

#endif // MYSTL_LIST_H_
