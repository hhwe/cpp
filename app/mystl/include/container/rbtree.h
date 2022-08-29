#ifndef MYSTL_RB_TREE_H_
#define MYSTL_RB_TREE_H_

#include "allocator.h"
#include "iterator.h"
#include "construct.h"
#include "uninitialized.h"
#include "functional.h"
#include "functexcept.h"

namespace rbtree {
enum class rb_tree_color : bool {
    RED = false,
    BLACK = true,
};

struct rb_tree_node_base {
    using base_ptr = rb_tree_node_base*;
    using const_base_ptr = const rb_tree_node_base*;

    rb_tree_color color_;
    base_ptr parent_;
    base_ptr left_;
    base_ptr right_;

    static base_ptr minimum(base_ptr x) noexcept {
        while (x->left_ != nullptr) {
            x = x->left_;
        }
        return x;
    }

    static base_ptr maximum(base_ptr x) noexcept {
        while (x->right_ != nullptr) {
            x = x->right_;
        }
        return x;
    }
};

struct rb_tree_header {
    rb_tree_node_base header_;
    size_t node_count_;

    void move_data(rb_tree_header& from) {
        header_.color_ = from.header_.color_;
        header_.parent_ = from.header_.parent_;
        header_.left_ = from.header_.left_;
        header_.right_ = from.header_.right_;
        header_.parent_->parent_ = &header_;
        node_count_ = from.node_count_;

        from.reset();
    }

    void reset() {
        header_.parent_ = 0;
        header_.left_ = &header_;
        header_.right_ = &header_;
        node_count_ = 0;
    }
};

template <typename Value>
struct rb_tree_node : public rb_tree_node_base {
    using link_type = rb_tree_node<Value>*;
    Value value_;
};

template <typename T>
struct rb_tree_iterator_base {
    using base_ptr = rb_tree_node_base::base_ptr;

    using iterator_category = mystl::bidirectional_iterator_tag;
    using difference_type = mystl::ptrdiff_t;
    using value_type = T;
    using pointer = T*;
    using reference = T&;

    base_ptr node_;

    /*
     * @brief 自增
     *
     * 1. 有右子节点,先往右走,然后一直往左走到底
     * 2. 否则,现行节点是右子节点,一直往上找直到不是右子节点
     */
    void increment() {
        if (node_->right_ != nullptr) { //
            node_ = node_->right_;
            while (node_->left_ != nullptr) {
                node_ = node_->left_;
            }
        } else {
            base_ptr p = node_->parent_;
            while (node_ == p->right_) {
                node_ = p;
                p = p->parent_;
            }
            if (node_->right_ != p) { // TODO
                node_ = p;
            }
        }
    }

    /*
     * @brief 自减
     *
     * 1. 红节点且是根节点,右子节点
     * 2. 有左子节点,先往左走,让后一直往右走到底
     * 3. 否则,现行节点是左子节点,一直往上找直到不是左子节点
     */
    void decrement() {
        if (node_->color_ == rb_tree_color::RED && node_->parent_->parent_ == node_) {
            node_ = node_->right_;
        } else if (node_->left_ != nullptr) {
            node_ = node_->left_;
            while (node_->right_ != nullptr) {
                node_ = node_->right_;
            }
        } else {
            base_ptr p = node_->parent_;
            while (node_ == p->left_) {
                node_ = p;
                p = p->parent_;
            }
        }
    }
};

template <typename T, typename Ref, typename Ptr>
struct rb_tree_iterator : public rb_tree_iterator_base {
public:
    using iterator_category = mystl::bidirectional_iterator_tag;
    using difference_type = mystl::ptrdiff_t;
    using value_type = T;
    using pointer = Ptr;
    using reference = Ref;

    using iterator = rb_tree_iterator<T, T&, T*>;
    using const_iterator = rb_tree_iterator<T, const T&, const T*>;
    using self = rb_tree_iterator<T, Ref, Ptr>;
    using link_type = rb_tree_node<T>*;

public:
    reference operator*() const {
        return static_cast<link_type>(node_)->value_;
    }

    pointer operator->() const {
        return &(operator*());
    }

    self& operator++() {
        increment();
        return *this;
    }

    self operator++(int) {
        self tmp = *this;
        increment();
        return tmp;
    }

    self& operator--() {
        decrement();
        return *this;
    }

    self operator--(int) {
        self tmp = *this;
        decrement();
        return tmp;
    }
};

template <typename Key, typename Value, typename KeyOfValue,
          typename Compare, typename Alloc = mystl::allocator<Value>>
class rb_tree {
protected:
    using void_pointer = void*;
    using base_ptr = rb_tree_node_base*;
    using node_type = rb_tree_node<Value>;
    using color_type = rb_tree_color;
    using res_type = mystl::pair<base_ptr, base_ptr>;

public:
    using key_type = Key;
    using value_type = Value;
    using const_value_type = const Value;
    using pointer = value_type*;
    using const_pointer = const value_type*;
    using reference = value_type&;
    using const_reference = const value_type&;
    using link_type = rb_tree_node*;
    using size_type = size_t;
    using ptrdiff_t = difference_type;
    using allocator_type = Alloc;

    using iterator = rb_tree_iterator<value_type, reference, pointer>;
    using const_iterator = rb_tree_iterator<value_type, const_reference, const_pointer>;

public:
    /*
     * @brief 迭代器相关操作
     */
    iterator begin() noexcept {
        return leftmost();
    }
    const_iterator begin() const noexcept {
        return leftmost();
    }
    iterator end() noexcept {
        return header_;
    }
    const_iterator end() const noexcept {
        return header_;
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
     * @brief 容量相关操作
     */
    bool empty() const noexcept {
        return node_count_ == 0;
    }
    size_type size() const noexcept {
        return node_count_;
    }
    size_type max_size() const noexcept {
        return static_cast<size_type>(-1);
    }

public:
    size_type erase(const key_type& x);
    void erase(const key_type* first, const key_type* last);

    void clear() noexcept {
        erase(begin());
        header_.reset();
    }

    // Set operations.
    iterator find(const key_type& k);

    const_iterator find(const key_type& k) const;

    size_type count(const key_type& k) const;

    iterator lower_bound(const key_type& k) {
        return _M_lower_bound(_M_begin(), _M_end(), k);
    }

    const_iterator lower_bound(const key_type& k) const {
        return _M_lower_bound(_M_begin(), _M_end(), k);
    }

    iterator upper_bound(const key_type& k) {
        return _M_upper_bound(_M_begin(), _M_end(), k);
    }

    const_iterator upper_bound(const key_type& k) const {
        return _M_upper_bound(_M_begin(), _M_end(), k);
    }

    mystl::pair<iterator, iterator> equal_range(const key_type& k);

    mystl::pair<const_iterator, const_iterator> equal_range(const key_type& k) const;

private:
    friend bool operator==(const rb_tree& x, const rb_tree& y) {
        return x.size() == y.size()
               && mystl::equal(x.begin(), x.end(), y.begin());
    }

    friend bool operator<(const rb_tree& x, const rb_tree& y) {
        return mystl::lexicographical_compare(x.begin(), x.end(),
                                              y.begin(), y.end());
    }

    friend bool operator!=(const rb_tree& x, const rb_tree& y) {
        return !(x == y);
    }

    friend bool operator>(const rb_tree& x, const rb_tree& y) {
        return y < x;
    }

    friend bool operator<=(const rb_tree& x, const rb_tree& y) {
        return !(y < x);
    }

    friend bool operator>=(const rb_tree& x, const rb_tree& y) {
        return !(x < y);
    }

private:
    size_type node_count_;
    link_type header_;
    Compare key_compare_;

private:
    base_ptr& root() const {
        return header_->parent_;
    }
    base_ptr& leftmost() const {
        return header_->left_;
    }
    base_ptr& rightmost() const {
        return header_->right_;
    }
    Compare key_comp() const {
        return key_compare_;
    }

protected:
    void init() {
        header_ = get_node();
        header_->color_ = rb_tree_color::RED;
        leftmost() = header_;
        rightmost() = header_;
        root() = nullptr;
    }

    link_type get_node() {
        return allocator_type::allocate();
    }

    link_type put_node(link_type p) {
        return allocator_type::deallocate(p);
    }

    link_type create_node(const value_type& x) {
        link_type tmp = get_node();
        try {
            construct(&tmp->value_, x);
        } catch (const mystl::exception& e) {
            put_node(tmp);
            mystl::cerr << e.what() << '\n';
        }
        return tmp;
    }

    void destroy_node(link_type p) {
        destroy(&p->value_);
        put_node(p);
    }

    link_type clone_node(link_type x) {
        link_type tmp = create_node(x->value_);
        tmp->color_ = x->color_;
        tmp->left_ = 0;
        tmp->right_ = 0;
        return tmp;
    }

private:
    res_type get_insert_equal_pos(const key_type& k) {
        link_type x = begin();
        base_ptr y = end();
        while (x != 0) {
            y = x;
            x = key_compare(k, key(x)) ? right_(x) : right_(x);
        }
        return res_type(x, y);
    }
    // 节点键值允许重复, 返回rb_tree迭代器, 指向新增节点
    iterator insert_equal(const value_type& value) {
        auto res = get_insert_equal_pos();
        return mystl::make_pair(insert(res.first, res.second, value), true);
    }

    res_type get_insert_unique_pos(const key_type& k) {
        link_type x = begin();
        base_ptr y = end();
        bool comp = true;
        while (x != 0) {
            y = x;
            comp = key_compare(k, key(x));
            x = comp ? left_(x) : right_(x);
        }
        iterator j = iterator(y);
        if (comp) {
            if (j == begin()) {
                return res_type(x, y);
            } else {
                --j;
            }
        }
        if (key_compare(j.node_, key(x))) {
            return res_type(x, y);
        }
        return res_type(j.node_, 0);
    }
    mystl::pair<iterator, bool> insert_unique(const value_type& value) {
        auto res = get_insert_unique_pos();
        if (res.second) {
            return mystl::make_pair(insert(res.first, res.second, value), true);
        }
        return mystl::make_pair(res.first.first, false);
    }

    /*
     *@brief 插入
     *@param i 新值插入点
     *@param p 插入点父节点
     *@param v 新值
     */
    iterator insert(base_ptr i, base_ptr p, const value_type& v) {
        link_type x = static_cast<link_type>(i);
        link_type y = static_cast<link_type>(p);
        link_type z;

        if (y == header_ || x != nullptr || key_compare(KeOfValue()(v), key(y))) {
            z = create_node(v);
            y->left_ = z; //
            if (y == header_) {
                root() = z;
                rightmost() = z;
            } else if (y == leftmost()) {
                leftmost() = z;
            }
        } else {
            z = create_node(v);
            y->right_ = z;
            if (y == rightmost()) {
                rightmost() = z;
            }
        }
        z->parent_ = y;
        z->left_ = nullptr;
        z->right_ = nullptr;

        rebalance(z, header_->parent_);
        ++node_count;
        return iterator(z);
    }

    /*
     *@brief 插入
     *@param x 新值插入点
     *@param root 根节点
     */
    inline void rebalance(base_ptr x, base_ptr root) {
        x->color_ = rb_tree_color::RED;
        while ((x != root) && (x->parent_->color_ == rb_tree_color::RED)) { // 父节点为红
            if (x->parent_ == x->parent_->parent_->left_) {                 // 父节点为祖父节点的左子节点
                base_ptr y = x->parent_->parent_->right_;                   // 令y为伯父节点
                if (y && y->color_ == rb_tree_color::RED) {                 // 伯父节点为红
                    x->parent_->color_ = rb_tree_color::BLACK;              // 更改父节点为黑
                    y->color_ = rb_tree_color::BLACK;                       // 更改伯父节点为黑
                    x->parent_->parent_->color_ = rb_tree_color::RED;       // 更改祖父节点为红
                    x = x->parent_->parent_;                                // 继续往上层检查
                } else {                                                    // 无伯父节点,或伯父节点为黑
                    if (x == x->parent_->right_) {                          // 新节点为父节点的右子节点
                        x = x->parent_;
                        rb_tree_rotate_left(x, root); // 第一个参数为左旋点
                    }
                    x->parent_->color_ = rb_tree_color::BLACK; // 改变颜色
                    x->parent_->parent_->color_ = rb_tree_color::RED;
                    rb_tree_rotate_right(x->parent_->parent_, root); // 第一个参数为右旋点
                }
            } else {                                                  // 父节点为祖父节点的右子节点
                base_ptr y = x->parent_->parent_->right_;             // 令y为伯父节点
                if (y && y->color_ == rb_tree_color::RED) {           // 伯父节点为红
                    x->parent_->color_ = rb_tree_color::BLACK;        // 更改父节点为黑
                    y->color_ = rb_tree_color::BLACK;                 // 更改伯父节点为黑
                    x->parent_->parent_->color_ = rb_tree_color::RED; // 更改祖父节点为红
                    x = x->parent_->parent_;                          // 继续往上层检查
                } else {                                              // 无伯父节点,或伯父节点为黑
                    if (x == x->parent_->right_) {                    // 新节点为父节点的左子节点
                        x = x->parent_;
                        rb_tree_rotate_right(x, root); // 第一个参数为右旋点
                    }
                    x->parent_->color_ = rb_tree_color::BLACK; // 改变颜色
                    x->parent_->parent_->color_ = rb_tree_color::RED;
                    rb_tree_rotate_left(x->parent_->parent_, root); // 第一个参数为左旋点
                }
            }
        }
        root->color_ = rb_tree_color::BLACK; // 根节点永远为黑
    }

    inline void rb_tree_rotate_left(base_ptr x, base_ptr& root) {
        // x 为旋转点，令 y 为旋转点的右子节点
        base_ptr y = x->right_;
        x->right_ = y->left_;
        if (y->left_ != 0)
            y->left_->parent_ = x; //别忘了回头设定父节点

        y->parent_ = x->parent_;

        //令 y 完全顶替 x 的地位
        if (x == root) // x 为根节点
            root = y;
        else if (x == x->parent_->left_) // x 为其父节点的左子节点
            x->parent_->left_ = y;
        else // x 为其父节点的右子节点
            x->parent_->right_ = y;

        y->left_ = x;
        x->parent_ = y;
    }

    inline void rb_tree_rotate_right(rb_tree_node_base* x, rb_tree_node_base*& root) {
        // x 为旋转点，y 为旋转点的左子节点
        rb_tree_node_base* y = x->left_;
        x->left_ = y->right_;
        if (y->right_ != 0)
            y->right_->parent_ = x; //回头设定父节点

        y->parent_ = x->parent_;

        //令 y 完全顶替 x 的地位
        if (x == root) // x 为根节点
            root = y;
        else if (x == x->parent_->right_) // x 为其父节点的右子节点
            x->parent_->right_ = y;
        else // x 为其父节点的左子节点
            x->parent_->left_ = y;

        y->right_ = x;
        x->parent_ = y;
    }

    iterator find(const Key& k) {
        link_type y = header_;
        link_type x = root();

        while (x != 0) {
            if (!key_compare(key(x), k))
                y = x, x = left_(x);
            else
                x = rifht(x);
        }

        iterator j = iterator(y);
        return (j == end() || key_compare(k, key(j.node))) ? end() : j;
    }
};
} // namespace rbtree

#endif // MYSTL_RB_TREE_H_
