#ifndef MYSTL_LIST_H_
#define MYSTL_LIST_H_

#include "allocator.h"
#include "iterator.h"
#include "construct.h"
#include "uninitialized.h"
#include "functional.h"
#include "rb_tree.h"
#include "utility.h"

namespace mystl {
template <typename Key, typename Compare = mystl::less<Key>, typename Alloc = mystl::allocator<Key>>
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

    using key_alloc_type = typename mystl::allocator_traits<Key, Alloc>::allocator_type;
    using rep_type = mystl::rb_tree<key_type, value_type, mystl::identity<value_type>, key_compare, key_alloc_type>;
    rep_type rb_tree_;

    using iterator = typename rep_type::iterator;
    using const_iterator = typename rep_type::const_iterator;
    using reverse_iterator = typename rep_type::reverse_iterator;
    using const_reverse_iterator = typename rep_type::const_reverse_iterator;

public: // member functions
    /*
     * @brief Constructor and Destructor
     */
    // empty(1)
    explicit set(const key_compare& comp = key_compare(),
                 const allocator_type& alloc = allocator_type()) :
        rb_tree_(comp, key_alloc_type(alloc)) {
    }
    explicit set(const allocator_type& alloc) :
        rb_tree_(key_alloc_type(alloc)) {
    }
    // range(2)
    template <class InputIterator>
    set(InputIterator first, InputIterator last,
        const key_compare& comp = key_compare(),
        const allocator_type& alloc = allocator_type()) :
        rb_tree_(comp, key_alloc_type(alloc)) {
        // rb_tree_.insert_range_unique(first, last);
    }
    // copy(3)
    set(const set& x) = default;
    set(const set& x, const allocator_type& alloc) :
        rb_tree_(x.rb_tree_, key_alloc_type(alloc)) {
    }
    // move(4)
    set(set&& x) = default;
    set(set&& x, const allocator_type& alloc) :
        rb_tree_(mystl::move(x.rb_tree_), key_alloc_type(alloc)) {
    }
    // initializer list(5)
    set(std::initializer_list<value_type> il,
        const key_compare& comp = key_compare(),
        const allocator_type& alloc = allocator_type()) :
        rb_tree_(comp, key_alloc_type(alloc)) {
        rb_tree_.insert_range_unique(il.begin(), il.end());
    }

    ~set() = default;

    // copy (1)
    set& operator=(const set& x) = default;
    // move (2)
    set& operator=(set&& x) = default;
    // initializer set (3)
    set& operator=(std::initializer_list<value_type> il) {
        rb_tree_.assign_unique(il.begin(), il.end());
        return *this;
    }

    /*
     *@brief Iterators
     */
    key_compare key_comp() const {
        return rb_tree_.key_comp();
    }
    value_compare value_comp() const {
        return rb_tree_.value_comp();
    }
    allocator_type get_allocator() const noexcept {
        return rb_tree_.get_allocator();
    }

    /*
     * @brief Iterators
     */
    iterator begin() noexcept {
        return rb_tree_.begin();
    }
    const_iterator begin() const noexcept {
        return rb_tree_.begin();
    }
    iterator end() noexcept {
        return rb_tree_.end();
    }
    const_iterator end() const noexcept {
        return rb_tree_.end();
    }
    reverse_iterator rbegin() noexcept {
        return rb_tree_.rbegin();
    }
    const_reverse_iterator rbegin() const noexcept {
        return rb_tree_.rbegin();
    }
    reverse_iterator rend() noexcept {
        return rb_tree_.rend();
    }
    const_reverse_iterator rend() const noexcept {
        return rb_tree_.rend();
    }
    const_iterator cbegin() const noexcept {
        return rb_tree_.cbegin();
    }
    const_iterator cend() const noexcept {
        return rb_tree_.cbegin();
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
        return rb_tree_.empty();
    }
    size_type size() const noexcept {
        return rb_tree_.size();
    }
    size_type max_size() const noexcept {
        return rb_tree_.max_size();
    }

    /*
     * @brief Modifiers
     */
    // single element (1)
    mystl::pair<iterator, bool> insert(const value_type& val) {
        mystl::pair<typename rep_type::iterator, bool> p = rb_tree_.insert_unique(val);
        return mystl::pair<iterator, bool>(p.first, p.second);
    }
    mystl::pair<iterator, bool> insert(value_type&& val) {
        mystl::pair<typename rep_type::iterator, bool> p = rb_tree_.insert_unique(mystl::move(val));
        return mystl::pair<iterator, bool>(p.first, p.second);
    }
    // with hint (2)
    // iterator insert(const_iterator position, const value_type& val) {
    iterator insert(iterator position, const value_type& val) {
        return rb_tree_.insert_unique(position, val);
    }
    iterator insert(iterator position, value_type&& val) {
        return rb_tree_.insert_unique(position, val);
    }
    // range (3)
    template <class InputIterator>
    void insert(InputIterator first, InputIterator last) {
        rb_tree_.insert_range_unique(first, last);
    }
    // initializer list (4)
    void insert(std::initializer_list<value_type> il) {
        rb_tree_.insert_range_unique(il.begin(), il.end());
    }

    iterator erase(const_iterator position) {
        return rb_tree_.erase(position);
    }
    size_type erase(const value_type& val) {
        return rb_tree_.erase(val);
    }
    iterator erase(const_iterator first, const_iterator last) {
        return rb_tree_.erase(first, last);
    }

    void swap(set& x) {
        rb_tree_.swap(x.rb_tree_);
    }

    void clear() noexcept {
        rb_tree_.clear();
    }

    template <class... Args>
    mystl::pair<iterator, bool> emplace(Args&&... args) {
        return rb_tree_.emplace_unique(mystl::forward<Args>(args)...);
    }
    template <class... Args>
    iterator emplace_hint(const_iterator position, Args&&... args) {
        return rb_tree_.emplace_hint_unique(position, mystl::forward<Args>(args)...);
    }

    /*
     * @brief Operations
     */
    iterator find(const key_type& val) {
        return rb_tree_.find(val);
    }
    const_iterator find(const key_type& val) const {
        return rb_tree_.find(val);
    }
    size_type count(const value_type& val) const {
        return rb_tree_.find(val) == rb_tree_.end() ? 0 : 1;
    }
    iterator lower_bound(const value_type& val) {
        return rb_tree_.lower_bound(val);
    }
    const_iterator lower_bound(const value_type& val) const {
        return rb_tree_.lower_bound(val);
    }
    iterator upper_bound(const value_type& val) {
        return rb_tree_.upper_bound(val);
    }
    const_iterator upper_bound(const value_type& val) const {
        return rb_tree_.upper_bound(val);
    }
    mystl::pair<const_iterator, const_iterator> equal_range(const value_type& val) const {
        return rb_tree_.equal_range(val);
    }
    mystl::pair<iterator, iterator> equal_range(const value_type& val) {
        return rb_tree_.equal_range(val);
    }
};
} // namespace mystl

#endif // MYSTL_LIST_H_
