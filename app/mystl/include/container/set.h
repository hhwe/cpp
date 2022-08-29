#ifndef MYSTL_LIST_H_
#define MYSTL_LIST_H_

#include "allocator.h"
#include "iterator.h"
#include "construct.h"
#include "uninitialized.h"
#include "functional.h"

namespace mystl {
template <typename Key typename Compare = mystl::less<Key>, typename Alloc = mystl::allocator<Key>>
class set {
public: // member types
    using allocator_type = Alloc;
    using key_type = Key;
    using key_compare = Compare;
    using value_compare = Compare;

    using value_type = typename allocator_type::value_type;
    using pointer = typename allocator_type::pointer;
    using const_pointer = typename allocator_type::const_pointer;
    using reference = typename allocator_type::reference;
    using const_reference = typename allocator_type::const_reference;
    using size_type = typename allocator_type::size_type;
    using difference_type = typename allocator_type::difference_type;

    using iterator = set_iterator<T, T&, T*>;
    using const_iterator = set_iterator<T, const T&, const T*>;
    using reverse_iterator = mystl::reverse_iterator<iterator>;
    using const_reverse_iterator = mystl::reverse_iterator<const_iterator>;

private:
    using key_alloc_type = typename mystl::allocator_traits<Alloc>::template rebind<Key>::other;
    using Rep_type = mystl::rb_tree<key_type, value_type, mystl::identity<value_type>, key_compare, key_alloc_type>;
    Rep_type rbtree_;

public: // member functions
    /*
     * @brief Constructor and Destructor
     */
    // empty(1)
    explicit set(const key_compare& comp = key_compare(),
                 const allocator_type& alloc = allocator_type()) :
        rbtree_(comp, key_alloc_type(alloc)) {
    }
    explicit set(const allocator_type& alloc) :
        rbtree_(key_alloc_type(alloc)) {
    }
    // range(2)
    template <class InputIterator>
    set(InputIterator first, InputIterator last,
        const key_compare& comp = key_compare(),
        const allocator_type& alloc = allocator_type()) :
        rbtree_(comp, key_alloc_type(alloc)) {
        rbtree_.insert_range_unique(first, last);
    }
    // copy(3)
    set(const set& x) = default;
    set(const set& x, const allocator_type& alloc) :
        rbtree_(x.rbtree_, key_alloc_type(alloc)) {
    }
    // move(4)
    set(set&& x) = default;
    set(set&& x, const allocator_type& alloc) :
        rbtree_(mystl::move(x.rbtree_), key_alloc_type(alloc)) {
    }
    // initializer list(5)
    set(initializer_list<value_type> il,
        const key_compare& comp = key_compare(),
        const allocator_type& alloc = allocator_type()) :
        rbtree_(comp, key_alloc_type(alloc)) {
        rbtree_.insert_range_unique(il.begin(), il.end());
    }

    ~set() = default;

    // copy (1)
    set& operator=(const set& x) = default;
    // move (2)
    set& operator=(set&& x) = default;
    // initializer set (3)
    set& operator=(std::initializer_set<value_type> il) {
        rbtree_.assign_unique(il.begin(), il.end());
        return *this;
    }

    /*
     *@brief Iterators
     */
    key_compare key_comp() const {
        return rbtree_.key_comp();
    }
    value_compare value_comp() const {
        return rbtree_.value_comp();
    }
    allocator_type get_allocator() const noexcept {
        return rbtree_.get_allocator();
    }

    /*
     * @brief Iterators
     */
    iterator begin() noexcept {
        return rbtree_.begin();
    }
    const_iterator begin() const noexcept {
        return rbtree_.begin();
    }
    iterator end() noexcept {
        return rbtree_.end();
    }
    const_iterator end() const noexcept {
        return rbtree_.end();
    }
    reverse_iterator rbegin() noexcept {
        return rbtree_.rbegin();
    }
    const_reverse_iterator rbegin() const noexcept {
        return rbtree_.rbegin();
    }
    reverse_iterator rend() noexcept {
        return rbtree_.rend();
    }
    const_reverse_iterator rend() const noexcept {
        return rbtree_.rend();
    }
    const_iterator cbegin() const noexcept {
        return rbtree_.cbegin();
    }
    const_iterator cend() const noexcept {
        return rbtree_.cbegin();
    }
    const_reverse_iterator crbegin() const noexcept {
        return rbegin();
    }
    const_reverse_iterator crend() const noexcept {
        return rend();
    }

    /*
     * @brief Capacity
     */
    bool empty() const noexcept {
        return rbtree_.empty();
    }
    size_type size() const noexcept {
        return rbtree_.size();
    }
    size_type max_size() const noexcept {
        return rbtree_.max_size();
    }

    /*
     * @brief Modifiers
     */
    // single element (1)
    mystl::pair<iterator, bool> insert(const value_type& val) {
        mystl::pair<typename Rep_type::iterator, bool> p = rbtree_.insert_unique(val);
        return myslt::pair<iterator, bool>(p.first, p.second);
    }
    mystl::pair<iterator, bool> insert(value_type&& val);
    // with hint (2)
    iterator insert(const_iterator position, const value_type& val) {
        return rbtree_.insert_unique(position, val);
    }
    iterator insert(const_iterator position, value_type&& val);
    // range (3)
    template <class InputIterator>
    void insert(InputIterator first, InputIterator last) {
        rbtree_.insert_range_unique(first, last);
    }
    // initializer list (4)
    void insert(initializer_list<value_type> il) {
        rbtree_.insert_range_unique(il.begin(), il.end());
    }

    iterator erase(const_iterator position) {
        return rbtree_.erase(position);
    }
    size_type erase(const value_type& val) {
        return rbtree_.erase(val);
    }
    iterator erase(const_iterator first, const_iterator last) {
        return rbtree_.erase(first, last);
    }

    void swap(set& x) {
        rbtree_.swap(x.rbtree_);
    }

    void clear() noexcept {
        rbtree_.clear();
    }

    template <class... Args>
    nystl::pair<iterator, bool> emplace(Args&&... args) {
        return rbtree_.emplace_unique(mystl::forward<Args>(args)...);
    }
    template <class... Args>
    iterator emplace_hint(const_iterator position, Args&&... args) {
        return rbtree_.emplace_hint_unique(position, mystl::forward<Args>(args)...);
    }

    /*
     * @brief Operations
     */
    iterator find(const key_type& val) {
        return rbtree_.find(val);
    }
    const_iterator find(const key_type& val) const {
        return rbtree_.find(val);
    }
    size_type count(const value_type& val) const {
        return rbtree_.find(val) == rbtree_.end() ? 0 : 1;
    }
    iterator lower_bound(const value_type& val) {
        return rbtree_.lower_bound(val);
    }
    const_iterator lower_bound(const value_type& val) const {
        return rbtree_.lower_bound(val);
    }
    iterator upper_bound(const value_type& val) {
        return rbtree_.upper_bound(val);
    }
    const_iterator upper_bound(const value_type& val) const {
        return rbtree_.upper_bound(val);
    }
    mystl::pair<const_iterator, const_iterator> equal_range(const value_type& val) const {
        return rbtree_.equal_range(val);
    }
    mystl::pair<iterator, iterator> equal_range(const value_type& val) {
        return rbtree_.equal_range(val);
    }
};
} // namespace mystl

#endif // MYSTL_LIST_H_
