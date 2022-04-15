#ifndef MYSTL_LIST_H_
#define MYSTL_LIST_H_

#include "allocator.h"
#include "construct.h"
#include "uninitialized.h"

namespace mystl {

struct ListNodeBase {
    ListNodeBase* next_;
    ListNodeBase* prev_;
};

template <typename T>
struct ListNode : public ListNodeBase {
    T data;
};

template <typename _Tp>
struct ListIterator {
};

template <typename _Tp>
struct ListConstIterator {
};

template <typename T, typename Alloc = mystl::allocator<T>>
class list {
public: // member types
    using allocator_type = Alloc;
    using data_allocator = Alloc;

    using value_type = typename allocator_type::value_type;
    using pointer = typename allocator_type::pointer;
    using const_pointer = typename allocator_type::const_pointer;
    using reference = typename allocator_type::reference;
    using const_reference = typename allocator_type::const_reference;
    using size_type = typename allocator_type::size_type;
    using difference_type = typename allocator_type::difference_type;

    using iterator = value_type*;
    using const_iterator = const value_type*;
    using reverse_iterator = mystl::reverse_iterator<iterator>;
    using const_reverse_iterator = mystl::reverse_iterator<const_iterator>;

public: // member functions
    /*
     * @brief Constructor and Destructor
     */
    // default (1)
    explicit list(const allocator_type& alloc = allocator_type());
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
    list(initializer_list<value_type> il,
         const allocator_type& alloc = allocator_type());

    ~list();

    // copy (1)
    list& operator=(const list& x);
    // move (2)
    list& operator=(list&& x);
    // initializer list (3)
    list& operator=(initializer_list<value_type> il);

protected:
    iterator first_;
    iterator finish_;
    iterator capacity_;
};

} // namespace mystl

#endif // MYSTL_LIST_H_
