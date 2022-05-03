#ifndef MYSTL_ITERATOR_H_
#define MYSTL_ITERATOR_H_

namespace mystl {

struct input_iterator_tag {};
struct output_iterator_tag {};
struct forward_iterator_tag : public input_iterator_tag {};
struct bidirectional_iterator_tag : public forward_iterator_tag {};
struct random_access_iterator_tag : public bidirectional_iterator_tag {};

template <typename Category, typename Tp, typename Distance = std::ptrdiff_t,
          typename Pointer = Tp*, typename Reference = Tp&>
struct iterator {
    using iterator_category = Category;
    using value_type = Tp;
    using difference_type = Distance;
    using pointer = Pointer;
    using reference = Reference;
};

// std::void_t功能：能够检测到应用SFINAE(替换失败并不是一个错误）特性时出现的非法类型。换句话说，给进来的类型 _Types 必须是一个有效的类型，不能是一个非法类型。
// 泛化版本
template <typename Iterator, typename = std::void_t<>>
struct iterator_traits_aux {};

// 特化版本, 如果没有如下的成员类型, 是个非法类型, 所以选择忽略特化版本, 从而实现泛化版本
template <typename Iterator>
struct iterator_traits_aux<Iterator, std::void_t<typename Iterator::iterator_category,
                                                 typename Iterator::value_type,
                                                 typename Iterator::difference_type,
                                                 typename Iterator::pointer,
                                                 typename Iterator::reference>> {
    using iterator_category = typename Iterator::iterator_category;
    using value_type = typename Iterator::value_type;
    using difference_type = typename Iterator::difference_type;
    using pointer = typename Iterator::pointer;
    using reference = typename Iterator::reference;
};

template <typename Iterator>
struct iterator_traits : public iterator_traits_aux<Iterator> {};

// 针对 pointer 的偏特化版本
template <typename Tp>
struct iterator_traits<Tp*> {
    using iterator_category = random_access_iterator_tag;
    using value_type = Tp;
    using difference_type = std::ptrdiff_t;
    using pointer = Tp*;
    using reference = Tp&;
};

// 针对 pointer-to-const 的偏特化版本
template <typename Tp>
struct iterator_traits<const Tp*> {
    using iterator_category = random_access_iterator_tag;
    using value_type = Tp;
    using difference_type = std::ptrdiff_t;
    using pointer = const Tp*;
    using reference = const Tp&;
};

// 萃取迭代器的类型
template <typename Iterator>
inline typename iterator_traits<Iterator>::iterator_category
iterator_category(const Iterator&) {
    using category = typename iterator_traits<Iterator>::iterator_category;
    return category();
}

// 萃取迭代器的元素类型
template <typename Iterator>
inline typename iterator_traits<Iterator>::value_type*
value_type(const Iterator&) {
    return static_cast<typename iterator_traits<Iterator>::value_type*>(0);
}

// 萃取迭代器的两个元素间距
template <typename Iterator>
inline typename iterator_traits<Iterator>::difference_type*
distance_type(const Iterator&) {
    return static_cast<typename iterator_traits<Iterator>::difference_type*>(0);
}

// ====================================================================================

// 反向迭代器
template <typename Iterator>
class reverse_iterator : public iterator_traits<Iterator> {
public: // member types
    using iterator_type = Iterator;
    using self = reverse_iterator<Iterator>;

    using iterator_category = typename iterator_traits<Iterator>::iterator_category;
    using value_type = typename iterator_traits<Iterator>::value_type;
    using difference_type = typename iterator_traits<Iterator>::difference_type;
    using pointer = typename iterator_traits<Iterator>::pointer;
    using reference = typename iterator_traits<Iterator>::reference;

public:
    reverse_iterator() {
    }
    explicit reverse_iterator(iterator_type x) :
        current_(x) {
    }

    iterator_type base() const {
        return current_;
    }

    reference operator*() const {
        iterator_type tmp = current_;
        return *--tmp;
    }

    pointer operator->() {
        return &(*this);
    }

    self& operator++() {
        --current_;
        return *this;
    }

    self& operator++(int) {
        self tmp = *this;
        --current_;
        return tmp;
    }

    self& operator--() {
        ++current_;
        return *this;
    }

    self& operator--(int) {
        self tmp = *this;
        ++current_;
        return tmp;
    }

private:
    iterator_type current_;
};

template <typename Iterator>
inline bool operator==(const reverse_iterator<Iterator>& x, const reverse_iterator<Iterator>& y) {
    return x.base() == y.base();
}

template <typename Iterator>
inline bool operator!=(const reverse_iterator<Iterator>& x, const reverse_iterator<Iterator>& y) {
    return !(x == y);
}

template <typename InputIterator>
using RequireInputIterator = typename std::enable_if<std::is_convertible<
    typename iterator_traits<InputIterator>::iterator_category,
    input_iterator_tag>::value>::type;

template <class InputIterator, class Distance>
void advance_dispatch(InputIterator& i, Distance n, input_iterator_tag) {
    while (n--)
        ++i;
}

template <class BidirectionalIterator, class Distance>
void advance_dispatch(BidirectionalIterator& i, Distance n, bidirectional_iterator_tag) {
    if (n >= 0)
        while (n--) ++i;
    else
        while (n++) --i;
}

template <class RandomIter, class Distance>
void advance_dispatch(RandomIter& i, Distance n, random_access_iterator_tag) {
    i += n;
}

template <class InputIterator, class Distance>
void advance(InputIterator& i, Distance n) {
    advance_dispatch(i, n, iterator_category(i));
}

template <class InputIterator>
typename mystl::iterator_traits<InputIterator>::difference_type
distance_dispatch(InputIterator first, InputIterator last, input_iterator_tag) {
    typename iterator_traits<InputIterator>::difference_type n = 0;
    while (first != last) {
        ++first;
        ++n;
    }
    return n;
}

template <class RandomIter>
typename mystl::iterator_traits<RandomIter>::difference_type
distance_dispatch(RandomIter first, RandomIter last,
                  random_access_iterator_tag) {
    return last - first;
}

template <class InputIterator>
typename iterator_traits<InputIterator>::difference_type
distance(InputIterator first, InputIterator last) {
    return distance_dispatch(first, last, iterator_category(first));
}

} // namespace mystl

#endif // MYSTL_ITERATOR_H_
