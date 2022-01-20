#ifndef MYSTL_UNINITIALIZED_H_
#define MYSTL_UNINITIALIZED_H_

namespace mystl {

template <class InputIterator, class ForwardIterator>
ForwardIterator uninitialized_copy(InputIterator first, InputIterator last,
                                   ForwardIterator result) {
    for (; first != last; ++result, ++first) {
        
    }
}

} // namespace mystl

#endif // MYSTL_UNINITIALIZED_H_
