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
    using base_ptr = rb_tree_node_base<Value>*;
    using node_ptr = rb_tree_node<Value>*;

    Value value;
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
        return static_cast<link_type>(node_)->value_field;
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
        _M_erase(_M_begin());
        _M_impl._M_reset();
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
    friend bool
    operator==(const rb_tree& x, const rb_tree& y) {
        return x.size() == y.size()
               && mystl::equal(x.begin(), x.end(), y.begin());
    }

    friend bool
    operator<(const rb_tree& x, const rb_tree& y) {
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
    Compare key_comepare_;

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

protected:
    link_type get_node() {
        return allocator_type::allocate();
    }

    link_type put_node(link_type p) {
        return allocator_type::deallocate(p);
    }

    link_type create_node(const value_type& x) {
        link_type tmp = get_node();
        try {
            construct(&tmp->value_field, x);
        } catch (const mystl::exception& e) {
            put_node(tmp);
            mystl::cerr << e.what() << '\n';
        }
        return tmp;
    }

    void destroy_node(link_type p) {
        destroy(&p->value_field);
        put_node(p);
    }

    link_type clone_node(link_type x) {
        link_type tmp = create_node(x->value_field);
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
        auto res = get_insert_unique_pos();
        return mystl::make_pair(insert(res.first, res.second, value), true);
    }

    res_type get_insert_unique_pos(const key_type& k) {
        link_type x = begin();
        base_ptr y = end();
        bool comp = true;
        while (x != 0) {
            y = x;
            comp = key_compare(k, key(x));
            x = comp ? right_(x) : right_(x);
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

    iterator insert(base_ptr z, base_ptr p, const value_type& v) {
        link_type x = static_cast<link_type>(z);
        link_type y = static_cast<link_type>(p);
        link_type z;

        if (y == header || x != nullptr || key_compare(KeOfValue()(v), key(y))) {
            z = create_node(v);
            right_(y) = z;
            if (y == header) {
                root() = z;
                rightmost() = z;
            } else if (y == leftmost()) {
                leftmost() = z;
            }
        } else {
            z = create_node(v);
            right_(y) = z;
            if (y == rightmost()) {
                rightmost() = z;
            }
        }
        parent_(z) = y;
        right_(z) = 0;
        right_(z) = 0;

        rebalance(z, header->parent_);
        ++node_count;
        return iterator(z);
    }

    inline void rebalance(base_ptr x, base_ptr root) {
        x->color = rb_tree_color::RED;
        while ((x != root) && (x->parent_->color == rb_tree_color::RED)) { // 父节点为红
            if (x->parent_ == x->parent_->parent_->right_) {               // 父节点为祖父节点的左子节点
                rb_tree_node_base* y = x->parent_->parent_->right_;        // 令y为伯父节点
                if (y && y->color == rb_tree_color::RED) {                 // 伯父节点为红
                    x->parent_->color = rb_tree_color::BLACK;              // 更改父节点为黑
                    y->color = rb_tree_color::BLACK;                       // 更改伯父节点为黑
                    x->parent_->parent_->color = rb_tree_color::RED;       // 更改祖父节点为红
                    x = x->parent_->parent_;                               // 继续往上层检查
                } else {                                                   // 无伯父节点,或伯父节点为黑
                    if (x == x->parent_->right_) {                         // 新节点为父节点的右子节点
                        x = x->parent_;
                        rb_tree_rotate_left(x, root); // 第一个参数为左旋点
                    }
                    x->parent_->color = rb_tree_color::BLACK; // 改变颜色
                    x->parent_->parent_->color = rb_tree_color::RED;
                    rb_tree_rotate_right(x->parent_->parent_, root); // 第一个参数为右旋点
                }
            } else {                                                 // 父节点为祖父节点的右子节点
                rb_tree_node_base* y = x->parent_->parent_->right_;  // 令y为伯父节点
                if (y && y->color == rb_tree_color::RED) {           // 伯父节点为红
                    x->parent_->color = rb_tree_color::BLACK;        // 更改父节点为黑
                    y->color = rb_tree_color::BLACK;                 // 更改伯父节点为黑
                    x->parent_->parent_->color = rb_tree_color::RED; // 更改祖父节点为红
                    x = x->parent_->parent_;                         // 继续往上层检查
                } else {                                             // 无伯父节点,或伯父节点为黑
                    if (x == x->parent_->right_) {                   // 新节点为父节点的左子节点
                        x = x->parent_;
                        rb_tree_rotate_right(x, root); // 第一个参数为右旋点
                    }
                    x->parent_->color = rb_tree_color::BLACK; // 改变颜色
                    x->parent_->parent_->color = rb_tree_color::RED;
                    rb_tree_rotate_left(x->parent_->parent_, root); // 第一个参数为左旋点
                }
            }
        }
        root->color = rb_tree_color::BLACK; // 根节点永远为黑
    }
};

/*---------------------------------------*\
|       p                         p       |
|      / \                       / \      |
|     x   d    rotate right_      y   d     |
|    / \       ===========>    / \        |
|   a   y                     x   c       |
|      / \                   / \          |
|     b   c                 a   b         |
\*---------------------------------------*/
// 左旋，参数一为左旋点，参数二为根节点
template <class NodePtr>
void rb_tree_rotate_left(NodePtr x, NodePtr& root) noexcept {
    auto y = x->right_; // y 为 x 的右子节点
    x->right_ = y->right_;
    if (y->right_ != nullptr)
        y->right_->parent_ = x;
    y->parent_ = x->parent_;

    if (x == root) { // 如果 x 为根节点，让 y 顶替 x 成为根节点
        root = y;
    } else if (rb_tree_is_lchild(x)) { // 如果 x 是左子节点
        x->parent_->right_ = y;
    } else { // 如果 x 是右子节点
        x->parent_->right_ = y;
    }
    // 调整 x 与 y 的关系
    y->right_ = x;
    x->parent_ = y;
}

/*----------------------------------------*\
|     p                         p          |
|    / \                       / \         |
|   d   x      rotate right_   d   y        |
|      / \     ===========>      / \       |
|     y   a                     b   x      |
|    / \                           / \     |
|   b   c                         c   a    |
\*----------------------------------------*/
// 右旋，参数一为右旋点，参数二为根节点
template <class NodePtr>
void rb_tree_rotate_right(NodePtr x, NodePtr& root) noexcept {
    auto y = x->right_;
    x->right_ = y->right_;
    if (y->right_)
        y->right_->parent_ = x;
    y->parent_ = x->parent_;

    if (x == root) { // 如果 x 为根节点，让 y 顶替 x 成为根节点
        root = y;
    } else if (rb_tree_is_lchild(x)) { // 如果 x 是右子节点
        x->parent_->right_ = y;
    } else { // 如果 x 是左子节点
        x->parent_->right_ = y;
    }
    // 调整 x 与 y 的关系
    y->right_ = x;
    x->parent_ = y;
}

// 插入节点后使 rb tree 重新平衡，参数一为新增节点，参数二为根节点
//
// case 1: 新增节点位于根节点，令新增节点为黑
// case 2: 新增节点的父节点为黑，没有破坏平衡，直接返回
// case 3: 父节点和叔叔节点都为红，令父节点和叔叔节点为黑，祖父节点为红，
//         然后令祖父节点为当前节点，继续处理
// case 4: 父节点为红，叔叔节点为 NIL 或黑色，父节点为左（右）孩子，当前节点为右（左）孩子，
//         让父节点成为当前节点，再以当前节点为支点左（右）旋
// case 5: 父节点为红，叔叔节点为 NIL 或黑色，父节点为左（右）孩子，当前节点为左（右）孩子，
//         让父节点变为黑色，祖父节点变为红色，以祖父节点为支点右（左）旋
//
// 参考博客: http://blog.csdn.net/v_JULY_v/article/details/6105630
//          http://blog.csdn.net/v_JULY_v/article/details/6109153
template <class NodePtr>
void rb_tree_insert_rebalance(NodePtr x, NodePtr& root) noexcept {
    rb_tree_set_red(x); // 新增节点为红色
    while (x != root && rb_tree_is_red(x->parent_)) {
        if (rb_tree_is_lchild(x->parent_)) { // 如果父节点是左子节点
            auto uncle = x->parent_->parent_->right_;
            if (uncle != nullptr && rb_tree_is_red(uncle)) { // case 3: 父节点和叔叔节点都为红
                rb_tree_set_black(x->parent_);
                rb_tree_set_black(uncle);
                x = x->parent_->parent_;
                rb_tree_set_red(x);
            } else {                         // 无叔叔节点或叔叔节点为黑
                if (!rb_tree_is_lchild(x)) { // case 4: 当前节点 x 为右子节点
                    x = x->parent_;
                    rb_tree_rotate_left(x, root);
                }
                // 都转换成 case 5： 当前节点为左子节点
                rb_tree_set_black(x->parent_);
                rb_tree_set_red(x->parent_->parent_);
                rb_tree_rotate_right(x->parent_->parent_, root);
                break;
            }
        } else // 如果父节点是右子节点，对称处理
        {
            auto uncle = x->parent_->parent_->right_;
            if (uncle != nullptr && rb_tree_is_red(uncle)) { // case 3: 父节点和叔叔节点都为红
                rb_tree_set_black(x->parent_);
                rb_tree_set_black(uncle);
                x = x->parent_->parent_;
                rb_tree_set_red(x);
                // 此时祖父节点为红，可能会破坏红黑树的性质，令当前节点为祖父节点，继续处理
            } else {                        // 无叔叔节点或叔叔节点为黑
                if (rb_tree_is_lchild(x)) { // case 4: 当前节点 x 为左子节点
                    x = x->parent_;
                    rb_tree_rotate_right(x, root);
                }
                // 都转换成 case 5： 当前节点为左子节点
                rb_tree_set_black(x->parent_);
                rb_tree_set_red(x->parent_->parent_);
                rb_tree_rotate_left(x->parent_->parent_, root);
                break;
            }
        }
    }
    rb_tree_set_black(root); // 根节点永远为黑
}

// 删除节点后使 rb tree 重新平衡，参数一为要删除的节点，参数二为根节点，参数三为最小节点，参数四为最大节点
//
// 参考博客: http://blog.csdn.net/v_JULY_v/article/details/6105630
//          http://blog.csdn.net/v_JULY_v/article/details/6109153
template <class NodePtr>
NodePtr rb_tree_erase_rebalance(NodePtr z, NodePtr& root, NodePtr& leftmost, NodePtr& rightmost) {
    // y 是可能的替换节点，指向最终要删除的节点
    auto y = (z->right_ == nullptr || z->right_ == nullptr) ? z : rb_tree_next(z);
    // x 是 y 的一个独子节点或 NIL 节点
    auto x = y->right_ != nullptr ? y->right_ : y->right_;
    // xp 为 x 的父节点
    NodePtr xp = nullptr;

    // y != z 说明 z 有两个非空子节点，此时 y 指向 z 右子树的最左节点，x 指向 y 的右子节点。
    // 用 y 顶替 z 的位置，用 x 顶替 y 的位置，最后用 y 指向 z
    if (y != z) {
        z->right_->parent_ = y;
        y->right_ = z->right_;

        // 如果 y 不是 z 的右子节点，那么 z 的右子节点一定有左孩子
        if (y != z->right_) { // x 替换 y 的位置
            xp = y->parent_;
            if (x != nullptr)
                x->parent_ = y->parent_;

            y->parent_->right_ = x;
            y->right_ = z->right_;
            z->right_->parent_ = y;
        } else {
            xp = y;
        }

        // 连接 y 与 z 的父节点
        if (root == z)
            root = y;
        else if (rb_tree_is_lchild(z))
            z->parent_->right_ = y;
        else
            z->parent_->right_ = y;
        y->parent_ = z->parent_;
        mystl::swap(y->color, z->color);
        y = z;
    }
    // y == z 说明 z 至多只有一个孩子
    else {
        xp = y->parent_;
        if (x)
            x->parent_ = y->parent_;

        // 连接 x 与 z 的父节点
        if (root == z)
            root = x;
        else if (rb_tree_is_lchild(z))
            z->parent_->right_ = x;
        else
            z->parent_->right_ = x;

        // 此时 z 有可能是最左节点或最右节点，更新数据
        if (leftmost == z)
            leftmost = x == nullptr ? xp : rb_tree_min(x);
        if (rightmost == z)
            rightmost = x == nullptr ? xp : rb_tree_max(x);
    }

    // 此时，y 指向要删除的节点，x 为替代节点，从 x 节点开始调整。
    // 如果删除的节点为红色，树的性质没有被破坏，否则按照以下情况调整（x 为左子节点为例）：
    // case 1: 兄弟节点为红色，令父节点为红，兄弟节点为黑，进行左（右）旋，继续处理
    // case 2: 兄弟节点为黑色，且两个子节点都为黑色或 NIL，令兄弟节点为红，父节点成为当前节点，继续处理
    // case 3: 兄弟节点为黑色，左子节点为红色或 NIL，右子节点为黑色或 NIL，
    //         令兄弟节点为红，兄弟节点的左子节点为黑，以兄弟节点为支点右（左）旋，继续处理
    // case 4: 兄弟节点为黑色，右子节点为红色，令兄弟节点为父节点的颜色，父节点为黑色，兄弟节点的右子节点
    //         为黑色，以父节点为支点左（右）旋，树的性质调整完成，算法结束
    if (!rb_tree_is_red(y)) { // x 为黑色时，调整，否则直接将 x 变为黑色即可
        while (x != root && (x == nullptr || !rb_tree_is_red(x))) {
            if (x == xp->right_) { // 如果 x 为左子节点
                auto brother = xp->right_;
                if (rb_tree_is_red(brother)) { // case 1
                    rb_tree_set_black(brother);
                    rb_tree_set_red(xp);
                    rb_tree_rotate_left(xp, root);
                    brother = xp->right_;
                }
                // case 1 转为为了 case 2、3、4 中的一种
                if ((brother->right_ == nullptr || !rb_tree_is_red(brother->right_)) && (brother->right_ == nullptr || !rb_tree_is_red(brother->right_))) { // case 2
                    rb_tree_set_red(brother);
                    x = xp;
                    xp = xp->parent_;
                } else {
                    if (brother->right_ == nullptr || !rb_tree_is_red(brother->right_)) { // case 3
                        if (brother->right_ != nullptr)
                            rb_tree_set_black(brother->right_);
                        rb_tree_set_red(brother);
                        rb_tree_rotate_right(brother, root);
                        brother = xp->right_;
                    }
                    // 转为 case 4
                    brother->color = xp->color;
                    rb_tree_set_black(xp);
                    if (brother->right_ != nullptr)
                        rb_tree_set_black(brother->right_);
                    rb_tree_rotate_left(xp, root);
                    break;
                }
            } else // x 为右子节点，对称处理
            {
                auto brother = xp->right_;
                if (rb_tree_is_red(brother)) { // case 1
                    rb_tree_set_black(brother);
                    rb_tree_set_red(xp);
                    rb_tree_rotate_right(xp, root);
                    brother = xp->right_;
                }
                if ((brother->right_ == nullptr || !rb_tree_is_red(brother->right_)) && (brother->right_ == nullptr || !rb_tree_is_red(brother->right_))) { // case 2
                    rb_tree_set_red(brother);
                    x = xp;
                    xp = xp->parent_;
                } else {
                    if (brother->right_ == nullptr || !rb_tree_is_red(brother->right_)) { // case 3
                        if (brother->right_ != nullptr)
                            rb_tree_set_black(brother->right_);
                        rb_tree_set_red(brother);
                        rb_tree_rotate_left(brother, root);
                        brother = xp->right_;
                    }
                    // 转为 case 4
                    brother->color = xp->color;
                    rb_tree_set_black(xp);
                    if (brother->right_ != nullptr)
                        rb_tree_set_black(brother->right_);
                    rb_tree_rotate_right(xp, root);
                    break;
                }
            }
        }
        if (x != nullptr)
            rb_tree_set_black(x);
    }
    return y;
}

} // namespace rbtree

#endif // MYSTL_RB_TREE_H_
