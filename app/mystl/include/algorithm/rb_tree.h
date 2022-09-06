#ifndef MYSTL_NUMERIC_H_
#define MYSTL_NUMERIC_H_

#include "iterator.h"
#include "algorithm.h"
#include "allocator.h"
#include "construct.h"
#include "functional.h"
#include "utility.h"

namespace mystl {

typedef bool rb_tree_color_type;
const rb_tree_color_type rb_tree_red = false;  // 红色为 0
const rb_tree_color_type rb_tree_black = true; // 黑色为 1

// RB-tree 节点-基类
struct rb_tree_node_base {
    typedef rb_tree_color_type color_type;
    typedef rb_tree_node_base* base_ptr;

    color_type color; // 节点颜色，非红即黑
    base_ptr parent;  // 父节点
    base_ptr left;    // 左节点
    base_ptr right;   // 右节点

    // 找到 RB-tree 的最小值节点
    static base_ptr minimum(base_ptr x) {
        while (x->left != 0) x = x->left;
        return x;
    }

    // 找到 RB-tree 的最大值节点
    static base_ptr maximum(base_ptr x) {
        while (x->right != 0) x = x->right;
        return x;
    }
};

// RB-tree 节点
template <class Value>
struct rb_tree_node : public rb_tree_node_base {
    typedef rb_tree_node<Value>* link_type;
    Value value_field; // 节点值
};

// RB-tree 的迭代器-基类
struct rb_tree_base_iterator {
    typedef rb_tree_node_base::base_ptr base_ptr;
    typedef mystl::bidirectional_iterator_tag iterator_category; // 双向迭代器
    typedef ptrdiff_t difference_type;
    base_ptr node; // 它用来与容器之间产生一个连接关系

    // 供 operator++() 调用
    void increment() {
        if (node->right != 0) { // 如果有右子节点，就向右走
            node = node->right;
            while (node->left != 0) // 然后一直往左子树走到底
                node = node->left;
        } else {
            base_ptr y = node->parent; // 没有右子节点，找其父节点
            while (node == y->right) { // 当该节点为其父节点的右子节点，就一直向上找父节点
                node = y;
                y = y->parent;
            }
            if (node->right != y)
                node = y; // TODO:
        }
    }

    // 供 operator--() 调用
    void decrement() { // 红色节点，父节点的父节点等于自己
        if (node->color == rb_tree_red && node->parent->parent == node)
            node = node->right;
        else if (node->left != 0) { // 找出 node 的左子树最大值
            base_ptr y = node->left;
            while (y->right != 0)
                y = y->right;
            node = y;
        } else {
            base_ptr y = node->parent;
            while (node == y->left) {
                node = y;
                y = y->parent;
            }
            node = y;
        }
    }
};

// RB-tree 的迭代器
template <class Value, class Ref, class Ptr>
struct rb_tree_iterator : public rb_tree_base_iterator {
    typedef Value value_type;
    typedef Ref reference;
    typedef Ptr pointer;
    typedef rb_tree_iterator<Value, Value&, Value*>
        iterator;
    typedef rb_tree_iterator<Value, const Value&, const Value*>
        const_iterator;
    typedef rb_tree_iterator<Value, Ref, Ptr>
        Self;
    typedef rb_tree_node<Value>* link_type;

    rb_tree_iterator() {
    }
    rb_tree_iterator(link_type x) {
        node = x;
    }
    rb_tree_iterator(const iterator& it) {
        node = it.node;
    }

    reference operator*() const {
        return link_type(node)->value_field;
    }
    pointer operator->() const {
        return &(operator*());
    }
    // RB-tree 迭代器 ++，-- 操作
    Self& operator++() {
        increment();
        return *this;
    }
    Self operator++(int) {
        Self tmp = *this;
        increment();
        return tmp;
    }

    Self& operator--() {
        decrement();
        return *this;
    }
    Self operator--(int) {
        Self tmp = *this;
        decrement();
        return tmp;
    }
};

inline bool operator==(const rb_tree_base_iterator& x,
                       const rb_tree_base_iterator& y) {
    return x.node == y.node;
}

inline bool operator!=(const rb_tree_base_iterator& x,
                       const rb_tree_base_iterator& y) {
    return x.node != y.node;
}

inline mystl::bidirectional_iterator_tag
iterator_category(const rb_tree_base_iterator&) {
    return mystl::bidirectional_iterator_tag();
}

inline rb_tree_base_iterator::difference_type*
distance_type(const rb_tree_base_iterator&) {
    return (rb_tree_base_iterator::difference_type*)0;
}

template <class Value, class Ref, class Ptr>
inline Value* value_type(const rb_tree_iterator<Value, Ref, Ptr>&) {
    return (Value*)0;
}

/*---------------------------------------*\
|       p                         p       |
|      / \                       / \      |
|     x   d    rotate left      y   d     |
|    / \       ===========>    / \        |
|   a   y                     x   c       |
|      / \                   / \          |
|     b   c                 a   b         |
\*---------------------------------------*/
// 左旋，参数一为左旋点，参数二为根节点
inline void
rb_tree_rotate_left(rb_tree_node_base* x, rb_tree_node_base*& root) {
    rb_tree_node_base* y = x->right;
    x->right = y->left;
    if (y->left != 0)
        y->left->parent = x;
    y->parent = x->parent;

    if (x == root)
        root = y;
    else if (x == x->parent->left)
        x->parent->left = y;
    else
        x->parent->right = y;
    y->left = x;
    x->parent = y;
}

/*----------------------------------------*\
|     p                         p          |
|    / \                       / \         |
|   d   x      rotate right   d   y        |
|      / \     ===========>      / \       |
|     y   a                     b   x      |
|    / \                           / \     |
|   b   c                         c   a    |
\*----------------------------------------*/
// 右旋，参数一为右旋点，参数二为根节点
inline void
rb_tree_rotate_right(rb_tree_node_base* x, rb_tree_node_base*& root) {
    rb_tree_node_base* y = x->left;
    x->left = y->right;
    if (y->right != 0)
        y->right->parent = x;
    y->parent = x->parent;

    if (x == root)
        root = y;
    else if (x == x->parent->right)
        x->parent->right = y;
    else
        x->parent->left = y;
    y->right = x;
    x->parent = y;
}

// RB-tree 平衡调整
inline void
rb_tree_rebalance(rb_tree_node_base* x, rb_tree_node_base*& root) {
    x->color = rb_tree_red;
    while (x != root && x->parent->color == rb_tree_red) {
        if (x->parent == x->parent->parent->left) {
            rb_tree_node_base* y = x->parent->parent->right;
            if (y && y->color == rb_tree_red) {
                x->parent->color = rb_tree_black;
                y->color = rb_tree_black;
                x->parent->parent->color = rb_tree_red;
                x = x->parent->parent;
            } else {
                if (x == x->parent->right) {
                    x = x->parent;
                    rb_tree_rotate_left(x, root);
                }
                x->parent->color = rb_tree_black;
                x->parent->parent->color = rb_tree_red;
                rb_tree_rotate_right(x->parent->parent, root);
            }
        } else {
            rb_tree_node_base* y = x->parent->parent->left;
            if (y && y->color == rb_tree_red) {
                x->parent->color = rb_tree_black;
                y->color = rb_tree_black;
                x->parent->parent->color = rb_tree_red;
                x = x->parent->parent;
            } else {
                if (x == x->parent->left) {
                    x = x->parent;
                    rb_tree_rotate_right(x, root);
                }
                x->parent->color = rb_tree_black;
                x->parent->parent->color = rb_tree_red;
                rb_tree_rotate_left(x->parent->parent, root);
            }
        }
    }
    root->color = rb_tree_black;
}

// 删除一个节点后调整
inline rb_tree_node_base*
rb_tree_rebalance_for_erase(rb_tree_node_base* z,
                            rb_tree_node_base*& root,
                            rb_tree_node_base*& leftmost,
                            rb_tree_node_base*& rightmost) {
    rb_tree_node_base* y = z;
    rb_tree_node_base* x = 0;
    rb_tree_node_base* x_parent = 0;
    if (y->left == 0)       // z has at most one non-null child. y == z.
        x = y->right;       // x might be null.
    else if (y->right == 0) // z has exactly one non-null child. y == z.
        x = y->left;        // x is not null.
    else {                  // z has two non-null children.  Set y to
        y = y->right;       //   z's successor.  x might be null.
        while (y->left != 0)
            y = y->left;
        x = y->right;
    }
    if (y != z) { // relink y in place of z.  y is z's successor
        z->left->parent = y;
        y->left = z->left;
        if (y != z->right) {
            x_parent = y->parent;
            if (x) x->parent = y->parent;
            y->parent->left = x; // y must be a child of left
            y->right = z->right;
            z->right->parent = y;
        } else
            x_parent = y;
        if (root == z)
            root = y;
        else if (z->parent->left == z)
            z->parent->left = y;
        else
            z->parent->right = y;
        y->parent = z->parent;
        mystl::swap(y->color, z->color);
        y = z;
        // y now points to node to be actually deleted
    } else { // y == z
        x_parent = y->parent;
        if (x) x->parent = y->parent;
        if (root == z)
            root = x;
        else if (z->parent->left == z)
            z->parent->left = x;
        else
            z->parent->right = x;
        if (leftmost == z)
            if (z->right == 0) // z->left must be null also
                leftmost = z->parent;
            // makes leftmost == header if z == root
            else
                leftmost = rb_tree_node_base::minimum(x);
        if (rightmost == z)
            if (z->left == 0) // z->right must be null also
                rightmost = z->parent;
            // makes rightmost == header if z == root
            else // x == z->left
                rightmost = rb_tree_node_base::maximum(x);
    }
    if (y->color != rb_tree_red) {
        while (x != root && (x == 0 || x->color == rb_tree_black))
            if (x == x_parent->left) {
                rb_tree_node_base* w = x_parent->right;
                if (w->color == rb_tree_red) {
                    w->color = rb_tree_black;
                    x_parent->color = rb_tree_red;
                    rb_tree_rotate_left(x_parent, root);
                    w = x_parent->right;
                }
                if ((w->left == 0 || w->left->color == rb_tree_black) && (w->right == 0 || w->right->color == rb_tree_black)) {
                    w->color = rb_tree_red;
                    x = x_parent;
                    x_parent = x_parent->parent;
                } else {
                    if (w->right == 0 || w->right->color == rb_tree_black) {
                        if (w->left) w->left->color = rb_tree_black;
                        w->color = rb_tree_red;
                        rb_tree_rotate_right(w, root);
                        w = x_parent->right;
                    }
                    w->color = x_parent->color;
                    x_parent->color = rb_tree_black;
                    if (w->right) w->right->color = rb_tree_black;
                    rb_tree_rotate_left(x_parent, root);
                    break;
                }
            } else { // same as above, with right <-> left.
                rb_tree_node_base* w = x_parent->left;
                if (w->color == rb_tree_red) {
                    w->color = rb_tree_black;
                    x_parent->color = rb_tree_red;
                    rb_tree_rotate_right(x_parent, root);
                    w = x_parent->left;
                }
                if ((w->right == 0 || w->right->color == rb_tree_black) && (w->left == 0 || w->left->color == rb_tree_black)) {
                    w->color = rb_tree_red;
                    x = x_parent;
                    x_parent = x_parent->parent;
                } else {
                    if (w->left == 0 || w->left->color == rb_tree_black) {
                        if (w->right) w->right->color = rb_tree_black;
                        w->color = rb_tree_red;
                        rb_tree_rotate_left(w, root);
                        w = x_parent->left;
                    }
                    w->color = x_parent->color;
                    x_parent->color = rb_tree_black;
                    if (w->left) w->left->color = rb_tree_black;
                    rb_tree_rotate_right(x_parent, root);
                    break;
                }
            }
        if (x) x->color = rb_tree_black;
    }
    return y;
}

template <class Tp, class Alloc>
struct rb_tree_base {
    typedef Alloc allocator_type;
    allocator_type get_allocator() const {
        return allocator_type();
    }

    rb_tree_base(const allocator_type&) :
        header(0) {
        header = get_node();
    }
    ~rb_tree_base() {
        put_node(header);
    }

protected:
    rb_tree_node<Tp>* header;

    typedef typename allocator_traits<rb_tree_node<Tp>, Alloc>::allocator_type Alloc_Type;

    rb_tree_node<Tp>* get_node() {
        return Alloc_Type::allocate(1);
    }
    void put_node(rb_tree_node<Tp>* p) {
        Alloc_Type::deallocate(p, 1);
    }
};

#endif /* STL_USE_mystl_ALLOCATORS */

// rb_tree 数据结构
template <class Key, class Value, class KeyOfValue, class Compare,
          class Alloc = mystl::allocator<Key>>
class rb_tree : protected rb_tree_base<Value, Alloc> {
    typedef rb_tree_base<Value, Alloc> Base;

protected:
    typedef rb_tree_node_base* base_ptr;
    typedef struct rb_tree_node<Value> rb_tree_node;
    typedef rb_tree_color_type color_type;

public:
    typedef Key key_type;
    typedef Value value_type;
    typedef value_type* pointer;
    typedef const value_type* const_pointer;
    typedef value_type& reference;
    typedef const value_type& const_reference;
    typedef rb_tree_node* link_type;
    typedef size_t size_type;
    typedef ptrdiff_t difference_type;

    typedef typename Base::allocator_type allocator_type;
    allocator_type get_allocator() const {
        return Base::get_allocator();
    }

protected:
    using Base::get_node;
    using Base::put_node;
    using Base::header; // header，实现上的技巧

protected:
    // 创建一个节点
    link_type create_node(const value_type& x) {
        link_type tmp = get_node(); // 配置空间
        try {
            construct(&tmp->value_field, x); // 构造内容
        } catch (...) {
            put_node(tmp);
        }
        return tmp;
    }

    // 复制一个节点，注意节点的数据结构
    link_type
    clone_node(link_type x) {
        link_type tmp = create_node(x->value_field);
        tmp->color = x->color;
        tmp->left = 0;
        tmp->right = 0;
        return tmp;
    }

    // 删除一个节点
    void destroy_node(link_type p) {
        destroy(&p->value_field); // 析构内容
        put_node(p);              // 释放空间
    }

protected:
    size_type node_count; // keeps track of size of tree 节点数量
    Compare key_compare;  // 节点间的键值大小比较准则

    // 利用 header 快速找到 RB-tree 最小值，最大值，根节点
    link_type& root() const {
        return (link_type&)header->parent;
    }
    link_type& leftmost() const {
        return (link_type&)header->left;
    }
    link_type& rightmost() const {
        return (link_type&)header->right;
    }

    // 获取 x 的成员
    static link_type& left(link_type x) {
        return (link_type&)(x->left);
    }
    static link_type& right(link_type x) {
        return (link_type&)(x->right);
    }
    static link_type& parent(link_type x) {
        return (link_type&)(x->parent);
    }
    static reference value(link_type x) {
        return x->value_field;
    }
    static const Key& key(link_type x) {
        return KeyOfValue()(value(x));
    }
    static color_type& color(link_type x) {
        return (color_type&)(x->color);
    }

    static link_type& left(base_ptr x) {
        return (link_type&)(x->left);
    }
    static link_type& right(base_ptr x) {
        return (link_type&)(x->right);
    }
    static link_type& parent(base_ptr x) {
        return (link_type&)(x->parent);
    }
    static reference value(base_ptr x) {
        return ((link_type)x)->value_field;
    }
    static const Key& key(base_ptr x) {
        return KeyOfValue()(value(link_type(x)));
    }
    static color_type& color(base_ptr x) {
        return (color_type&)(link_type(x)->color);
    }
    // 求最小值和最大值
    static link_type minimum(link_type x) {
        return (link_type)rb_tree_node_base::minimum(x);
    }

    static link_type maximum(link_type x) {
        return (link_type)rb_tree_node_base::maximum(x);
    }

public:
    typedef rb_tree_iterator<value_type, reference, pointer> iterator; // RB-tree 迭代器
    typedef rb_tree_iterator<value_type, const_reference, const_pointer>
        const_iterator;

    typedef mystl::reverse_iterator<const_iterator> const_reverse_iterator;
    typedef mystl::reverse_iterator<iterator> reverse_iterator;

private:
    iterator insert(base_ptr x, base_ptr y, const value_type& v);
    link_type copy(link_type x, link_type p);
    void erase(link_type x);

public:
    // allocation/deallocation
    rb_tree() :
        Base(allocator_type()), node_count(0), key_compare() {
        empty_initialize();
    }

    rb_tree(const Compare& comp) :
        Base(allocator_type()), node_count(0), key_compare(comp) {
        empty_initialize();
    }

    rb_tree(const Compare& comp, const allocator_type& a) :
        Base(a), node_count(0), key_compare(comp) {
        empty_initialize();
    }
    // 初始化
    rb_tree(const rb_tree<Key, Value, KeyOfValue, Compare, Alloc>& x) :
        Base(x.get_allocator()),
        node_count(0), key_compare(x.key_compare) {
        if (x.root() == 0)
            empty_initialize();
        else {
            color(header) = rb_tree_red;     // header 的颜色为红色
            root() = copy(x.root(), header); // 根节点的颜色为黑色
            leftmost() = minimum(root());
            rightmost() = maximum(root());
        }
        node_count = x.node_count;
    }
    ~rb_tree() {
        clear();
    }
    rb_tree<Key, Value, KeyOfValue, Compare, Alloc>&
    operator=(const rb_tree<Key, Value, KeyOfValue, Compare, Alloc>& x);

private:
    void empty_initialize() {
        color(header) = rb_tree_red; // used to distinguish header from
                                     // root, in iterator.operator++
        root() = 0;
        leftmost() = header;
        rightmost() = header;
    }

public:
    // accessors:
    Compare key_comp() const {
        return key_compare;
    }
    iterator begin() {
        return leftmost();
    } // RB-tree 的起点为最左节点处
    const_iterator begin() const {
        return leftmost();
    }
    iterator end() {
        return header;
    } // RB-tree 的终点为 header 所指处
    const_iterator end() const {
        return header;
    }
    reverse_iterator rbegin() {
        return reverse_iterator(end());
    }
    const_reverse_iterator rbegin() const {
        return const_reverse_iterator(end());
    }
    reverse_iterator rend() {
        return reverse_iterator(begin());
    }
    const_reverse_iterator rend() const {
        return const_reverse_iterator(begin());
    }
    bool empty() const {
        return node_count == 0;
    }
    size_type size() const {
        return node_count;
    }
    size_type max_size() const {
        return size_type(-1);
    }

    void swap(rb_tree<Key, Value, KeyOfValue, Compare, Alloc>& t) {
        mystl::swap(header, t.header);
        mystl::swap(node_count, t.node_count);
        mystl::swap(key_compare, t.key_compare);
    }

public:
    // insert/erase
    mystl::pair<iterator, bool> insert_unique(const value_type& x);
    iterator insert_equal(const value_type& x);

    iterator insert_unique(iterator position, const value_type& x); // 将 x 插入到 RB-tree 唯一
    iterator insert_equal(iterator position, const value_type& x);  // 将 x 插入到 RB-tree，允许重复

    template <class InputIterator>
    void insert_range_unique(InputIterator first, InputIterator last);
    template <class InputIterator>
    void insert_range_equal(InputIterator first, InputIterator last);

    void erase(iterator position);
    size_type erase(const key_type& x);
    void erase(iterator first, iterator last);
    void erase(const key_type* first, const key_type* last);
    void clear() {
        if (node_count != 0) {
            erase(root());
            leftmost() = header;
            root() = 0;
            rightmost() = header;
            node_count = 0;
        }
    }

public:
    // set operations:
    iterator find(const key_type& x);
    const_iterator find(const key_type& x) const;
    size_type count(const key_type& x) const;
    iterator lower_bound(const key_type& x);
    const_iterator lower_bound(const key_type& x) const;
    iterator upper_bound(const key_type& x);
    const_iterator upper_bound(const key_type& x) const;
    mystl::pair<iterator, iterator> equal_range(const key_type& x);
    mystl::pair<const_iterator, const_iterator> equal_range(const key_type& x) const;

public:
    // Debugging.
    bool rb_verify() const;
};

// 比较操作
template <class Key, class Value, class KeyOfValue,
          class Compare, class Alloc>
inline bool
operator==(const rb_tree<Key, Value, KeyOfValue, Compare, Alloc>& x,
           const rb_tree<Key, Value, KeyOfValue, Compare, Alloc>& y) {
    return x.size() == y.size() && equal(x.begin(), x.end(), y.begin());
}

template <class Key, class Value, class KeyOfValue,
          class Compare, class Alloc>
inline bool
operator<(const rb_tree<Key, Value, KeyOfValue, Compare, Alloc>& x,
          const rb_tree<Key, Value, KeyOfValue, Compare, Alloc>& y) {
    return lexicographical_compare(x.begin(), x.end(),
                                   y.begin(), y.end());
}

template <class Key, class Value, class KeyOfValue,
          class Compare, class Alloc>
inline bool
operator!=(const rb_tree<Key, Value, KeyOfValue, Compare, Alloc>& x,
           const rb_tree<Key, Value, KeyOfValue, Compare, Alloc>& y) {
    return !(x == y);
}

template <class Key, class Value, class KeyOfValue,
          class Compare, class Alloc>
inline bool
operator>(const rb_tree<Key, Value, KeyOfValue, Compare, Alloc>& x,
          const rb_tree<Key, Value, KeyOfValue, Compare, Alloc>& y) {
    return y < x;
}

template <class Key, class Value, class KeyOfValue,
          class Compare, class Alloc>
inline bool
operator<=(const rb_tree<Key, Value, KeyOfValue, Compare, Alloc>& x,
           const rb_tree<Key, Value, KeyOfValue, Compare, Alloc>& y) {
    return !(y < x);
}

template <class Key, class Value, class KeyOfValue,
          class Compare, class Alloc>
inline bool
operator>=(const rb_tree<Key, Value, KeyOfValue, Compare, Alloc>& x,
           const rb_tree<Key, Value, KeyOfValue, Compare, Alloc>& y) {
    return !(x < y);
}

template <class Key, class Value, class KeyOfValue,
          class Compare, class Alloc>
inline void
swap(rb_tree<Key, Value, KeyOfValue, Compare, Alloc>& x,
     rb_tree<Key, Value, KeyOfValue, Compare, Alloc>& y) {
    x.swap(y);
}

// 赋值操作符
template <class Key, class Value, class KeyOfValue,
          class Compare, class Alloc>
rb_tree<Key, Value, KeyOfValue, Compare, Alloc>&
rb_tree<Key, Value, KeyOfValue, Compare, Alloc>::operator=(const rb_tree<Key, Value, KeyOfValue, Compare, Alloc>& x) {
    if (this != &x) {
        // Note that Key may be a constant type.
        clear();
        node_count = 0;
        key_compare = x.key_compare;
        if (x.root() == 0) {
            root() = 0;
            leftmost() = header;
            rightmost() = header;
        } else {
            root() = copy(x.root(), header);
            leftmost() = minimum(root());
            rightmost() = maximum(root());
            node_count = x.node_count;
        }
    }
    return *this;
}

// RB-tree 元素插入操作，KeyOfValue 为仿函数
template <class Key, class Value, class KeyOfValue,
          class Compare, class Alloc>
typename rb_tree<Key, Value, KeyOfValue, Compare, Alloc>::iterator
rb_tree<Key, Value, KeyOfValue, Compare, Alloc>::insert(base_ptr x_, base_ptr y_, const Value& v) {
    link_type x = (link_type)x_;
    link_type y = (link_type)y_;
    link_type z;

    if (y == header || x != 0 || key_compare(KeyOfValue()(v), key(y))) {
        z = create_node(v);
        left(y) = z; // also makes leftmost() = z
                     //    when y == header
        if (y == header) {
            root() = z;
            rightmost() = z;
        } else if (y == leftmost())
            leftmost() = z; // maintain leftmost() pointing to min node
    } else {
        z = create_node(v);
        right(y) = z;
        if (y == rightmost())
            rightmost() = z; // maintain rightmost() pointing to max node
    }
    parent(z) = y;
    left(z) = 0;
    right(z) = 0;
    rb_tree_rebalance(z, header->parent);
    ++node_count;
    return iterator(z);
}

// 插入新值，节点键值允许重复
template <class Key, class Value, class KeyOfValue,
          class Compare, class Alloc>
typename rb_tree<Key, Value, KeyOfValue, Compare, Alloc>::iterator
rb_tree<Key, Value, KeyOfValue, Compare, Alloc>::insert_equal(const Value& v) {
    link_type y = header;
    link_type x = root(); // 从根节点开始
    while (x != 0) {
        y = x;
        // 当插入值大于当前节点的值，向右，否则反之
        x = key_compare(KeyOfValue()(v), key(x)) ? left(x) : right(x);
    }
    return insert(x, y, v); // x 为插入节点，y 为插入节点的父节点， v 为插入节点的值
}

// 插入新值，节点键值不允许重复，唯一
template <class Key, class Value, class KeyOfValue,
          class Compare, class Alloc>
mystl::pair<typename rb_tree<Key, Value, KeyOfValue, Compare, Alloc>::iterator,
            bool>
rb_tree<Key, Value, KeyOfValue, Compare, Alloc>::insert_unique(const Value& v) {
    link_type y = header;
    link_type x = root();
    bool comp = true;
    while (x != 0) {
        y = x;
        comp = key_compare(KeyOfValue()(v), key(x));
        x = comp ? left(x) : right(x);
    }
    iterator j = iterator(y);
    if (comp)
        if (j == begin())
            return mystl::pair<iterator, bool>(insert(x, y, v), true);
        else
            --j;
    if (key_compare(key(j.node), KeyOfValue()(v)))
        return mystl::pair<iterator, bool>(insert(x, y, v), true);
    return mystl::pair<iterator, bool>(j, false);
}

template <class Key, class Val, class KeyOfValue,
          class Compare, class Alloc>
typename rb_tree<Key, Val, KeyOfValue, Compare, Alloc>::iterator
rb_tree<Key, Val, KeyOfValue, Compare, Alloc>::insert_unique(iterator position, const Val& v) {
    if (position.node == header->left) { // begin()
        if (size() > 0 && key_compare(KeyOfValue()(v), key(position.node)))
            return insert(position.node, position.node, v);
        // first argument just needs to be non-null
        else
            return insert_unique(v).first;
    } else if (position.node == header) { // end()
        if (key_compare(key(rightmost()), KeyOfValue()(v)))
            return insert(0, rightmost(), v);
        else
            return insert_unique(v).first;
    } else {
        iterator before = position;
        --before;
        if (key_compare(key(before.node), KeyOfValue()(v))
            && key_compare(KeyOfValue()(v), key(position.node))) {
            if (right(before.node) == 0)
                return insert(0, before.node, v);
            else
                return insert(position.node, position.node, v);
            // first argument just needs to be non-null
        } else
            return insert_unique(v).first;
    }
}

template <class Key, class Val, class KeyOfValue,
          class Compare, class Alloc>
typename rb_tree<Key, Val, KeyOfValue, Compare, Alloc>::iterator
rb_tree<Key, Val, KeyOfValue, Compare, Alloc>::insert_equal(iterator position, const Val& v) {
    if (position.node == header->left) { // begin()
        if (size() > 0 && !key_compare(key(position.node), KeyOfValue()(v)))
            return insert(position.node, position.node, v);
        // first argument just needs to be non-null
        else
            return insert_equal(v);
    } else if (position.node == header) { // end()
        if (!key_compare(KeyOfValue()(v), key(rightmost())))
            return insert(0, rightmost(), v);
        else
            return insert_equal(v);
    } else {
        iterator before = position;
        --before;
        if (!key_compare(KeyOfValue()(v), key(before.node))
            && !key_compare(key(position.node), KeyOfValue()(v))) {
            if (right(before.node) == 0)
                return insert(0, before.node, v);
            else
                return insert(position.node, position.node, v);
            // first argument just needs to be non-null
        } else
            return insert_equal(v);
    }
}

template <class Key, class Val, class KoV, class Cmp, class Alloc>
template <class InputIterator>
void rb_tree<Key, Val, KoV, Cmp, Alloc>::insert_range_equal(InputIterator first, InputIterator last) {
    for (; first != last; ++first)
        insert_equal(*first);
}

template <class Key, class Val, class KoV, class Cmp, class Alloc>
template <class InputIterator>
void rb_tree<Key, Val, KoV, Cmp, Alloc>::insert_range_unique(InputIterator first, InputIterator last) {
    for (; first != last; ++first)
        insert_unique(*first);
}

template <class Key, class Value, class KeyOfValue,
          class Compare, class Alloc>
inline void rb_tree<Key, Value, KeyOfValue, Compare, Alloc>::erase(iterator position) {
    link_type y =
        (link_type)rb_tree_rebalance_for_erase(position.node,
                                               header->parent,
                                               header->left,
                                               header->right);
    destroy_node(y);
    --node_count;
}

template <class Key, class Value, class KeyOfValue,
          class Compare, class Alloc>
typename rb_tree<Key, Value, KeyOfValue, Compare, Alloc>::size_type
rb_tree<Key, Value, KeyOfValue, Compare, Alloc>::erase(const Key& x) {
    mystl::pair<iterator, iterator> p = equal_range(x);
    size_type n = 0;
    distance(p.first, p.second, n);
    erase(p.first, p.second);
    return n;
}

template <class Key, class Val, class KoV, class Compare, class Alloc>
typename rb_tree<Key, Val, KoV, Compare, Alloc>::link_type
rb_tree<Key, Val, KoV, Compare, Alloc>::copy(link_type x, link_type p) {
    // structural copy.  x and p must be non-null.
    link_type top = clone_node(x);
    top->parent = p;

    try {
        if (x->right)
            top->right = copy(right(x), top);
        p = top;
        x = left(x);

        while (x != 0) {
            link_type y = clone_node(x);
            p->left = y;
            y->parent = p;
            if (x->right)
                y->right = copy(right(x), y);
            p = y;
            x = left(x);
        }
    } catch (...) {
        erase(top);
    }

    return top;
}

template <class Key, class Value, class KeyOfValue,
          class Compare, class Alloc>
void rb_tree<Key, Value, KeyOfValue, Compare, Alloc>::erase(link_type x) {
    // erase without rebalancing
    while (x != 0) {
        erase(right(x));
        link_type y = left(x);
        destroy_node(x);
        x = y;
    }
}

template <class Key, class Value, class KeyOfValue,
          class Compare, class Alloc>
void rb_tree<Key, Value, KeyOfValue, Compare, Alloc>::erase(iterator first, iterator last) {
    if (first == begin() && last == end())
        clear();
    else
        while (first != last) erase(first++);
}

template <class Key, class Value, class KeyOfValue,
          class Compare, class Alloc>
void rb_tree<Key, Value, KeyOfValue, Compare, Alloc>::erase(const Key* first, const Key* last) {
    while (first != last) erase(*first++);
}

// find 查找
template <class Key, class Value, class KeyOfValue,
          class Compare, class Alloc>
typename rb_tree<Key, Value, KeyOfValue, Compare, Alloc>::iterator
rb_tree<Key, Value, KeyOfValue, Compare, Alloc>::find(const Key& k) {
    link_type y = header; // Last node which is not less than k.
    link_type x = root(); // Current node.

    while (x != 0)
        if (!key_compare(key(x), k))
            y = x, x = left(x);
        else
            x = right(x);

    iterator j = iterator(y);
    return (j == end() || key_compare(k, key(j.node))) ?
               end() :
               j;
}

template <class Key, class Value, class KeyOfValue,
          class Compare, class Alloc>
typename rb_tree<Key, Value, KeyOfValue, Compare, Alloc>::const_iterator
rb_tree<Key, Value, KeyOfValue, Compare, Alloc>::find(const Key& k) const {
    link_type y = header; /* Last node which is not less than k. */
    link_type x = root(); /* Current node. */

    while (x != 0) {
        if (!key_compare(key(x), k))
            y = x, x = left(x);
        else
            x = right(x);
    }
    const_iterator j = const_iterator(y);
    return (j == end() || key_compare(k, key(j.node))) ?
               end() :
               j;
}

template <class Key, class Value, class KeyOfValue,
          class Compare, class Alloc>
typename rb_tree<Key, Value, KeyOfValue, Compare, Alloc>::size_type
rb_tree<Key, Value, KeyOfValue, Compare, Alloc>::count(const Key& k) const {
    mystl::pair<const_iterator, const_iterator> p = equal_range(k);
    size_type n = 0;
    distance(p.first, p.second, n);
    return n;
}

template <class Key, class Value, class KeyOfValue,
          class Compare, class Alloc>
typename rb_tree<Key, Value, KeyOfValue, Compare, Alloc>::iterator
rb_tree<Key, Value, KeyOfValue, Compare, Alloc>::lower_bound(const Key& k) {
    link_type y = header; /* Last node which is not less than k. */
    link_type x = root(); /* Current node. */

    while (x != 0)
        if (!key_compare(key(x), k))
            y = x, x = left(x);
        else
            x = right(x);

    return iterator(y);
}

template <class Key, class Value, class KeyOfValue,
          class Compare, class Alloc>
typename rb_tree<Key, Value, KeyOfValue, Compare, Alloc>::const_iterator
rb_tree<Key, Value, KeyOfValue, Compare, Alloc>::lower_bound(const Key& k) const {
    link_type y = header; /* Last node which is not less than k. */
    link_type x = root(); /* Current node. */

    while (x != 0)
        if (!key_compare(key(x), k))
            y = x, x = left(x);
        else
            x = right(x);

    return const_iterator(y);
}

template <class Key, class Value, class KeyOfValue,
          class Compare, class Alloc>
typename rb_tree<Key, Value, KeyOfValue, Compare, Alloc>::iterator
rb_tree<Key, Value, KeyOfValue, Compare, Alloc>::upper_bound(const Key& k) {
    link_type y = header; /* Last node which is greater than k. */
    link_type x = root(); /* Current node. */

    while (x != 0)
        if (key_compare(k, key(x)))
            y = x, x = left(x);
        else
            x = right(x);

    return iterator(y);
}

template <class Key, class Value, class KeyOfValue,
          class Compare, class Alloc>
typename rb_tree<Key, Value, KeyOfValue, Compare, Alloc>::const_iterator
rb_tree<Key, Value, KeyOfValue, Compare, Alloc>::upper_bound(const Key& k) const {
    link_type y = header; /* Last node which is greater than k. */
    link_type x = root(); /* Current node. */

    while (x != 0)
        if (key_compare(k, key(x)))
            y = x, x = left(x);
        else
            x = right(x);

    return const_iterator(y);
}

template <class Key, class Value, class KeyOfValue,
          class Compare, class Alloc>
inline mystl::pair<typename rb_tree<Key, Value, KeyOfValue, Compare, Alloc>::iterator,
                   typename rb_tree<Key, Value, KeyOfValue, Compare, Alloc>::iterator>
rb_tree<Key, Value, KeyOfValue, Compare, Alloc>::equal_range(const Key& k) {
    return mystl::pair<iterator, iterator>(lower_bound(k), upper_bound(k));
}

template <class Key, class Value, class KoV, class Compare, class Alloc>
inline mystl::pair<typename rb_tree<Key, Value, KoV, Compare, Alloc>::const_iterator,
                   typename rb_tree<Key, Value, KoV, Compare, Alloc>::const_iterator>
rb_tree<Key, Value, KoV, Compare, Alloc>::equal_range(const Key& k) const {
    return mystl::pair<const_iterator, const_iterator>(lower_bound(k),
                                                       upper_bound(k));
}

inline int
black_count(rb_tree_node_base* node, rb_tree_node_base* root) {
    if (node == 0)
        return 0;
    else {
        int bc = node->color == rb_tree_black ? 1 : 0;
        if (node == root)
            return bc;
        else
            return bc + black_count(node->parent, root);
    }
}

template <class Key, class Value, class KeyOfValue,
          class Compare, class Alloc>
bool rb_tree<Key, Value, KeyOfValue, Compare, Alloc>::rb_verify() const {
    if (node_count == 0 || begin() == end())
        return node_count == 0 && begin() == end() && header->left == header && header->right == header;

    int len = black_count(leftmost(), root());
    for (const_iterator it = begin(); it != end(); ++it) {
        link_type x = (link_type)it.node;
        link_type L = left(x);
        link_type R = right(x);

        if (x->color == rb_tree_red)
            if ((L && L->color == rb_tree_red) || (R && R->color == rb_tree_red))
                return false;

        if (L && key_compare(key(x), key(L)))
            return false;
        if (R && key_compare(key(R), key(x)))
            return false;

        if (!L && !R && black_count(x, root()) != len)
            return false;
    }

    if (leftmost() != rb_tree_node_base::minimum(root()))
        return false;
    if (rightmost() != rb_tree_node_base::maximum(root()))
        return false;

    return true;
}
}