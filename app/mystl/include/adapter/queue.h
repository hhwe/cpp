#ifndef MYSTL_QUEUE_H_
#define MYSTL_QUEUE_H_

#include <cstddef>

#include "deque.h"
#include "vector.h"
#include "algorithm.h"
#include "allocator.h"
#include "utility.h"

namespace mystl {
template <typename T, typename Container = mystl::deque<T>>
class queue {
    template <typename T1, typename Container1>
    friend bool
    operator==(const queue<T1, Container1>&, const queue<T1, Container1>&);

    template <typename T1, typename Container1>
    friend bool
    operator<(const queue<T1, Container1>&, const queue<T1, Container1>&);

public: // member types
    using container_type = Container;

    using value_type = typename container_type::value_type;
    using reference = typename container_type::reference;
    using const_reference = typename container_type::const_reference;
    using size_type = typename container_type::size_type;

public: // member functions
    /*
     * @brief Constructor and Destructor
     */
    // nitialize (1)
    explicit queue(const container_type& ctnr) :
        ctnr_(ctnr) {
    }
    // move-initialize (2)
    explicit queue(container_type&& ctnr = container_type()) :
        ctnr_(mystl::move(ctnr)) {
    }
    // allocator (3)
    template <class Alloc>
    explicit queue(const Alloc& alloc) :
        ctnr_(alloc) {
    }
    // init + allocator (4)
    template <class Alloc>
    queue(const container_type& ctnr, const Alloc& alloc) :
        ctnr_(ctnr, alloc) {
    }
    // move-init + allocator (5)
    template <class Alloc>
    queue(container_type&& ctnr, const Alloc& alloc) :
        ctnr_(mystl::move(ctnr), alloc) {
    }
    // copy + allocator (6)
    template <class Alloc>
    queue(const queue& x, const Alloc& alloc) :
        ctnr_(x.ctnr_, alloc) {
    }
    // move + allocator (7)
    template <class Alloc>
    queue(queue&& x, const Alloc& alloc) :
        ctnr_(std::move(x.ctnr_), alloc) {
    }

    /*
     * @brief Capacity
     */
    size_type size() const noexcept {
        return ctnr_.size();
    }

    bool empty() const noexcept {
        return ctnr_.empty();
    }

    /*
     * @brief Element access
     */
    reference front() {
        return ctnr_.front();
    }
    const_reference front() const;

    reference back() {
        return ctnr_.back();
    }
    const_reference back() const;

    /*
     * @brief Modifiers
     */

    template <class... Args>
    void emplace(Args&&... args) {
        ctnr_.emplace_back(mystl::forward<Args>(args)...);
    }

    void push(const value_type& val) {
        ctnr_.push_back(val);
    }
    void push(value_type&& val) {
        ctnr_.push_back(std::move(val));
    }

    void pop() {
        ctnr_.pop_front();
    }

    void swap(queue& x) {
        mystl::swap(ctnr_, x.ctnr_);
    }

protected:
    container_type ctnr_; // 底层容器
};

// (1)
template <class T, class Container>
bool operator==(const queue<T, Container>& lhs, const queue<T, Container>& rhs) {
    return lhs.ctnr_ == rhs.ctnr_;
}
// (2)
template <class T, class Container>
bool operator!=(const queue<T, Container>& lhs, const queue<T, Container>& rhs) {
    return !(lhs == rhs);
}
// (3)
template <class T, class Container>
bool operator<(const queue<T, Container>& lhs, const queue<T, Container>& rhs);
// (4)
template <class T, class Container>
bool operator<=(const queue<T, Container>& lhs, const queue<T, Container>& rhs);
// (5)
template <class T, class Container>
bool operator>(const queue<T, Container>& lhs, const queue<T, Container>& rhs);
// (6)
template <class T, class Container>
bool operator>=(const queue<T, Container>& lhs, const queue<T, Container>& rhs);

template <class T, class Container>
void swap(queue<T, Container>& x, queue<T, Container>& y) {
    x.swap(y);
}

template <typename T, typename Container = mystl::vector<T>, typename Compare = mystl::less<typename Container::value_type>>
class priority_queue {
public: // member types
    using container_type = Container;

    using value_type = typename container_type::value_type;
    using reference = typename container_type::reference;
    using const_reference = typename container_type::const_reference;
    using size_type = typename container_type::size_type;

public: // member functions
    /*
     * @brief Constructor and Destructor
     */
    // initialize (1)
    priority_queue(const Compare& comp, const Container& ctnr) :
        ctnr_(ctnr), comp_(comp) {
        mystl::make_heap(ctnr_.begin(), ctnr_.end(), comp);
    }
    // range (2)
    template <class InputIterator>
    priority_queue(InputIterator first, InputIterator last, const Compare& comp, const Container& ctnr) :
        ctnr_(ctnr), comp_(comp) {
        ctnr_.insert(ctnr_.end(), first, last);
        mystl::make_heap(ctnr_.begin(), ctnr_.end(), comp);
    }
    // move-initialize (3)
    explicit priority_queue(const Compare& comp = Compare(), Container&& ctnr = Container()) :
        ctnr_(mystl::move(ctnr)), comp_(comp) {
        mystl::make_heap(ctnr_.begin(), ctnr_.end(), comp);
    }
    // move-range (4)
    template <class InputIterator>
    priority_queue(InputIterator first, InputIterator last, const Compare& comp = Compare(), Container&& ctnr = Container()) :
        ctnr_(mystl::move(ctnr)), comp_(comp) {
        ctnr_.insert(ctnr_.end(), first, last);
        mystl::make_heap(ctnr_.begin(), ctnr_.end(), comp);
    }
    // allocator versions (5)
    template <class Alloc>
    explicit priority_queue(const Alloc& alloc) :
        ctnr_(alloc), comp_() {
    }
    template <class Alloc>
    priority_queue(const Compare& comp, const Alloc& alloc) :
        ctnr_(alloc), comp_(comp) {
    }
    template <class Alloc>
    priority_queue(const Compare& comp, const Container& ctnr, const Alloc& alloc) :
        ctnr_(ctnr, alloc), comp_(comp) {
        mystl::make_heap(ctnr_.begin(), ctnr_.end(), comp);
    }
    template <class Alloc>
    priority_queue(const Compare& comp, Container&& ctnr, const Alloc& alloc) :
        ctnr_(mystl::move(ctnr), alloc), comp_(comp) {
        mystl::make_heap(ctnr_.begin(), ctnr_.end(), comp);
    }
    template <class Alloc>
    priority_queue(const priority_queue& x, const Alloc& alloc) :
        ctnr_(x.ctnr_, alloc), comp_(x.comp_) {
    }
    template <class Alloc>
    priority_queue(priority_queue&& x, const Alloc& alloc) :
        ctnr_(std::move(x.ctnr_), alloc), comp_(std::move(x.comp_)) {
    }

    /*
     * @brief Capacity
     */
    size_type size() const noexcept {
        return ctnr_.size();
    }

    bool empty() const noexcept {
        return ctnr_.empty();
    }

    /*
     * @brief Element access
     */
    const_reference top() const {
        return ctnr_.front();
    }

    /*
     * @brief Modifiers
     */

    template <class... Args>
    void emplace(Args&&... args) {
        ctnr_.emplace_back(mystl::forward<Args>(args)...);
        mystl::push_heap(ctnr_.begin(), ctnr_.end(), comp_);
    }

    void push(const value_type& val) {
        ctnr_.push_back(val);
        mystl::push_heap(ctnr_.begin(), ctnr_.end(), comp_);
    }
    void push(value_type&& val) {
        ctnr_.push_back(std::move(val));
        mystl::push_heap(ctnr_.begin(), ctnr_.end(), comp_);
    }

    void pop() {
        mystl::pop_heap(ctnr_.begin(), ctnr_.end(), comp_);
        ctnr_.pop_back();
    }

    void swap(priority_queue& x) {
        mystl::swap(ctnr_, x.ctnr_);
        mystl::swap(comp_, x.comp_);
    }

protected:
    container_type ctnr_;
    Compare comp_;
};

template <typename T, typename Container, typename Compare>
void swap(priority_queue<T, Container, Compare>& x, priority_queue<T, Container, Compare>& y) {
    x.swap(y);
}
} // namespace mystl

#endif // MYSTL_QUEUE_H_
