#ifndef MYSTL_HASHTABLE_H_
#define MYSTL_HASHTABLE_H_

// Hashtable class, used to implement the hashed associative containers
// hash_set, hash_map, hash_multiset, and hash_multimap.

#include "vector.h"
#include "iterator.h"
#include "algorithm.h"
#include "functional.h"
#include "hash_fun.h"

namespace mystl {

using mystl::size_t;
using mystl::ptrdiff_t;
using mystl::forward_iterator_tag;
using mystl::input_iterator_tag;
using mystl::_Construct;
using mystl::_Destroy;
using mystl::distance;
using mystl::vector;
using mystl::mystl::pair;
using mystl::iterator_category;

template <class Val>
struct Hashtable_node {
    Hashtable_node* next;
    Val val;
};

template <class Val, class Key, class HashFcn, class ExtractKey,
          class EqualKey, class Alloc = mystl::allocator<Val>>
class hashtable;

template <class Val, class Key, class HashFcn,
          class ExtractKey, class EqualKey, class Alloc>
struct Hashtable_iterator;

template <class Val, class Key, class HashFcn,
          class ExtractKey, class EqualKey, class Alloc>
struct Hashtable_const_iterator;

template <class Val, class Key, class HashFcn,
          class ExtractKey, class EqualKey, class Alloc>
struct Hashtable_iterator {
    typedef hashtable<Val, Key, HashFcn, ExtractKey, EqualKey, Alloc> Hashtable;
    typedef Hashtable_iterator<Val, Key, HashFcn, ExtractKey, EqualKey, Alloc> iterator;
    typedef Hashtable_const_iterator<Val, Key, HashFcn, ExtractKey, EqualKey, Alloc> const_iterator;
    typedef Hashtable_node<Val> Node;
    typedef mystl::forward_iterator_tag iterator_category;
    typedef Val value_type;
    typedef ptrdiff_t difference_type;
    typedef size_t size_type;
    typedef Val& reference;
    typedef Val* pointer;

    Node* cur;
    Hashtable* ht;

    Hashtable_iterator(Node* n, Hashtable* tab) :
        cur(n), ht(tab) {
    }

    Hashtable_iterator() {
    }

    reference
    operator*() const {
        return cur->val;
    }

    pointer
    operator->() const {
        return &(operator*());
    }

    iterator&
    operator++();

    iterator
    operator++(int);

    bool
    operator==(const iterator& it) const {
        return cur == it.cur;
    }

    bool
    operator!=(const iterator& it) const {
        return cur != it.cur;
    }
};

template <class Val, class Key, class HashFcn,
          class ExtractKey, class EqualKey, class Alloc>
struct Hashtable_const_iterator {
    typedef hashtable<Val, Key, HashFcn, ExtractKey, EqualKey, Alloc> Hashtable;
    typedef Hashtable_iterator<Val, Key, HashFcn, ExtractKey, EqualKey, Alloc> iterator;
    typedef Hashtable_const_iterator<Val, Key, HashFcn, ExtractKey, EqualKey, Alloc> const_iterator;
    typedef Hashtable_node<Val> Node;

    typedef mystl::forward_iterator_tag iterator_category;
    typedef Val value_type;
    typedef ptrdiff_t difference_type;
    typedef size_t size_type;
    typedef const Val& reference;
    typedef const Val* pointer;

    const Node* cur;
    const Hashtable* ht;

    Hashtable_const_iterator(const Node* n, const Hashtable* tab) :
        cur(n), ht(tab) {
    }

    Hashtable_const_iterator() {
    }

    Hashtable_const_iterator(const iterator& it) :
        cur(it.cur), ht(it.ht) {
    }

    reference
    operator*() const {
        return cur->val;
    }

    pointer
    operator->() const {
        return &(operator*());
    }

    const_iterator&
    operator++();

    const_iterator
    operator++(int);

    bool
    operator==(const const_iterator& it) const {
        return cur == it.cur;
    }

    bool
    operator!=(const const_iterator& it) const {
        return cur != it.cur;
    }
};

// Note: assumes long is at least 32 bits.
enum { num_primes = 29 };

template <typename PrimeType>
struct Hashtable_prime_list {
    static const PrimeType stl_prime_list[num_primes];

    static const PrimeType*
    get_prime_list();
};

template <typename PrimeType>
const PrimeType
    Hashtable_prime_list<PrimeType>::stl_prime_list[num_primes] =
        {
            5ul, 53ul, 97ul, 193ul, 389ul,
            769ul, 1543ul, 3079ul, 6151ul, 12289ul,
            24593ul, 49157ul, 98317ul, 196613ul, 393241ul,
            786433ul, 1572869ul, 3145739ul, 6291469ul, 12582917ul,
            25165843ul, 50331653ul, 100663319ul, 201326611ul, 402653189ul,
            805306457ul, 1610612741ul, 3221225473ul, 4294967291ul};

template <class PrimeType>
inline const PrimeType*
Hashtable_prime_list<PrimeType>::get_prime_list() {
    return stl_prime_list;
}

inline unsigned long
stl_next_prime(unsigned long n) {
    const unsigned long* first = Hashtable_prime_list<unsigned long>::get_prime_list();
    const unsigned long* last = first + (int)num_primes;
    const unsigned long* pos = mystl::lower_bound(first, last, n);
    return pos == last ? *(last - 1) : *pos;
}

// Forward declaration of operator==.
template <class Val, class Key, class HF, class Ex,
          class Eq, class All>
class hashtable;

template <class Val, class Key, class HF, class Ex,
          class Eq, class All>
bool operator==(const hashtable<Val, Key, HF, Ex, Eq, All>& ht1,
                const hashtable<Val, Key, HF, Ex, Eq, All>& ht2);

// Hashtables handle allocators a bit differently than other
// containers do.  If we're using standard-conforming allocators, then
// a hashtable unconditionally has a member variable to hold its
// allocator, even if it so happens that all instances of the
// allocator type are identical.  This is because, for hashtables,
// this extra storage is negligible.  Additionally, a base class
// wouldn't serve any other purposes; it wouldn't, for example,
// simplify the exception-handling code.
template <class Val, class Key, class HashFcn,
          class ExtractKey, class EqualKey, class Alloc>
class hashtable {
public:
    typedef Key key_type;
    typedef Val value_type;
    typedef HashFcn hasher;
    typedef EqualKey key_equal;

    typedef size_t size_type;
    typedef ptrdiff_t difference_type;
    typedef value_type* pointer;
    typedef const value_type* const_pointer;
    typedef value_type& reference;
    typedef const value_type& const_reference;

    hasher
    hash_funct() const {
        return hash;
    }

    key_equal
    key_eq() const {
        return equals;
    }

private:
    typedef Hashtable_node<Val> Node;

public:
    typedef typename Alloc::template rebind<value_type>::other allocator_type;
    allocator_type
    get_allocator() const {
        return node_allocator;
    }

private:
    typedef typename Alloc::template rebind<Node>::other NodeAlloc;
    typedef typename Alloc::template rebind<Node*>::other NodeptrAlloc;
    typedef vector<Node*, NodeptrAlloc> Vector_type;

    NodeAlloc node_allocator;

    Node*
    get_node() {
        return node_allocator.allocate(1);
    }

    void
    put_node(Node* p) {
        node_allocator.deallocate(p, 1);
    }

private:
    hasher hash;
    key_equal equals;
    ExtractKey get_key;
    Vector_type buckets;
    size_type num_elements;

public:
    typedef Hashtable_iterator<Val, Key, HashFcn, ExtractKey,
                               EqualKey, Alloc>
        iterator;
    typedef Hashtable_const_iterator<Val, Key, HashFcn, ExtractKey,
                                     EqualKey, Alloc>
        const_iterator;

    friend struct
        Hashtable_iterator<Val, Key, HashFcn, ExtractKey, EqualKey, Alloc>;

    friend struct
        Hashtable_const_iterator<Val, Key, HashFcn, ExtractKey,
                                 EqualKey, Alloc>;

public:
    hashtable(size_type n, const HashFcn& hf,
              const EqualKey& eql, const ExtractKey& ext,
              const allocator_type& a = allocator_type()) :
        node_allocator(a),
        hash(hf), equals(eql),
        get_key(ext), buckets(a), num_elements(0) {
        initialize_buckets(n);
    }

    hashtable(size_type n, const HashFcn& hf,
              const EqualKey& eql,
              const allocator_type& a = allocator_type()) :
        node_allocator(a),
        hash(hf), equals(eql),
        get_key(ExtractKey()), buckets(a), num_elements(0) {
        initialize_buckets(n);
    }

    hashtable(const hashtable& ht) :
        node_allocator(ht.get_allocator()), hash(ht.hash),
        equals(ht.equals), get_key(ht.get_key),
        buckets(ht.get_allocator()), num_elements(0) {
        copy_from(ht);
    }

    hashtable&
    operator=(const hashtable& ht) {
        if (&ht != this) {
            clear();
            hash = ht.hash;
            equals = ht.equals;
            get_key = ht.get_key;
            copy_from(ht);
        }
        return *this;
    }

    ~hashtable() {
        clear();
    }

    size_type
    size() const {
        return num_elements;
    }

    size_type
    max_size() const {
        return size_type(-1);
    }

    _GLIBCXX_NODISCARD bool
    empty() const {
        return size() == 0;
    }

    void
    swap(hashtable& ht) {
        mystl::swap(hash, ht.hash);
        mystl::swap(equals, ht.equals);
        mystl::swap(get_key, ht.get_key);
        buckets.swap(ht.buckets);
        mystl::swap(num_elements, ht.num_elements);
    }

    iterator
    begin() {
        for (size_type n = 0; n < buckets.size(); ++n)
            if (buckets[n])
                return iterator(buckets[n], this);
        return end();
    }

    iterator
    end() {
        return iterator(0, this);
    }

    const_iterator
    begin() const {
        for (size_type n = 0; n < buckets.size(); ++n)
            if (buckets[n])
                return const_iterator(buckets[n], this);
        return end();
    }

    const_iterator
    end() const {
        return const_iterator(0, this);
    }

    template <class Vl, class Keyy, class HF, class Ex, class Eq,
              class Al>
    friend bool
    operator==(const hashtable<Vl, Keyy, HF, Ex, Eq, Al>&,
               const hashtable<Vl, Keyy, HF, Ex, Eq, Al>&);

public:
    size_type
    bucket_count() const {
        return buckets.size();
    }

    size_type
    max_bucket_count() const {
        return Hashtable_prime_list<unsigned long>::
            get_prime_list()[(int)num_primes - 1];
    }

    size_type
    elems_in_bucket(size_type bucket) const {
        size_type result = 0;
        for (Node* n = buckets[bucket]; n; n = n->next)
            result += 1;
        return result;
    }

    mystl::pair<iterator, bool>
    insert_unique(const value_type& obj) {
        resize(num_elements + 1);
        return insert_unique_noresize(obj);
    }

    iterator
    insert_equal(const value_type& obj) {
        resize(num_elements + 1);
        return insert_equal_noresize(obj);
    }

    mystl::pair<iterator, bool>
    insert_unique_noresize(const value_type& obj);

    iterator
    insert_equal_noresize(const value_type& obj);

    template <class _InputIterator>
    void
    insert_unique(_InputIterator f, _InputIterator l) {
        insert_unique(f, l, iterator_category(f));
    }

    template <class _InputIterator>
    void
    insert_equal(_InputIterator f, _InputIterator l) {
        insert_equal(f, l, iterator_category(f));
    }

    template <class _InputIterator>
    void
    insert_unique(_InputIterator f, _InputIterator l,
                  input_iterator_tag) {
        for (; f != l; ++f)
            insert_unique(*f);
    }

    template <class _InputIterator>
    void
    insert_equal(_InputIterator f, _InputIterator l,
                 input_iterator_tag) {
        for (; f != l; ++f)
            insert_equal(*f);
    }

    template <class ForwardIterator>
    void
    insert_unique(ForwardIterator f, ForwardIterator l,
                  mystl::forward_iterator_tag) {
        size_type n = distance(f, l);
        resize(num_elements + n);
        for (; n > 0; --n, ++f)
            insert_unique_noresize(*f);
    }

    template <class ForwardIterator>
    void
    insert_equal(ForwardIterator f, ForwardIterator l,
                 mystl::forward_iterator_tag) {
        size_type n = distance(f, l);
        resize(num_elements + n);
        for (; n > 0; --n, ++f)
            insert_equal_noresize(*f);
    }

    reference
    find_or_insert(const value_type& obj);

    iterator
    find(const key_type& key) {
        size_type n = bkt_num_key(key);
        Node* first;
        for (first = buckets[n];
             first && !equals(get_key(first->val), key);
             first = first->next) {}
        return iterator(first, this);
    }

    const_iterator
    find(const key_type& key) const {
        size_type n = bkt_num_key(key);
        const Node* first;
        for (first = buckets[n];
             first && !equals(get_key(first->val), key);
             first = first->next) {}
        return const_iterator(first, this);
    }

    size_type
    count(const key_type& key) const {
        const size_type n = bkt_num_key(key);
        size_type result = 0;

        for (const Node* cur = buckets[n]; cur;
             cur = cur->next)
            if (equals(get_key(cur->val), key))
                ++result;
        return result;
    }

    mystl::pair<iterator, iterator>
    equal_range(const key_type& key);

    mystl::pair<const_iterator, const_iterator>
    equal_range(const key_type& key) const;

    size_type
    erase(const key_type& key);

    void
    erase(const iterator& it);

    void
    erase(iterator first, iterator last);

    void
    erase(const const_iterator& it);

    void
    erase(const_iterator first, const_iterator last);

    void
    resize(size_type num_elements_hint);

    void
    clear();

private:
    size_type next_size(size_type n) const {
        return stl_next_prime(n);
    }

    void initialize_buckets(size_type n) {
        const size_type n_buckets = next_size(n);
        buckets.reserve(n_buckets);
        buckets.insert(buckets.end(), n_buckets, (Node*)0);
        num_elements = 0;
    }

    size_type bkt_num_key(const key_type& key) const {
        return bkt_num_key(key, buckets.size());
    }

    size_type bkt_num(const value_type& obj) const {
        return bkt_num_key(get_key(obj));
    }

    size_type bkt_num_key(const key_type& key, size_t n) const {
        return hash(key) % n;
    }

    size_type bkt_num(const value_type& obj, size_t n) const {
        return bkt_num_key(get_key(obj), n);
    }

    Node* new_node(const value_type& obj) {
        Node* n = get_node();
        n->next = 0;
        try {
            this->get_allocator().construct(&n->val, obj);
            return n;
        } catch (...) {
            put_node(n);
            throw_exception_again;
        }
    }

    void delete_node(Node* n) {
        this->get_allocator().destroy(&n->val);
        put_node(n);
    }

    void erase_bucket(const size_type n, Node* first, Node* last);

    void erase_bucket(const size_type n, Node* last);

    void copy_from(const hashtable& ht);
};

template <class Val, class Key, class HF, class ExK, class EqK,
          class All>
Hashtable_iterator<Val, Key, HF, ExK, EqK, All>&
Hashtable_iterator<Val, Key, HF, ExK, EqK, All>::
operator++() {
    const Node* old = cur;
    cur = cur->next;
    if (!cur) {
        size_type bucket = ht->bkt_num(old->val);
        while (!cur && ++bucket < ht->buckets.size())
            cur = ht->buckets[bucket];
    }
    return *this;
}

template <class Val, class Key, class HF, class ExK, class EqK,
          class All>
inline Hashtable_iterator<Val, Key, HF, ExK, EqK, All>
Hashtable_iterator<Val, Key, HF, ExK, EqK, All>::
operator++(int) {
    iterator tmp = *this;
    ++*this;
    return tmp;
}

template <class Val, class Key, class HF, class ExK, class EqK,
          class All>
Hashtable_const_iterator<Val, Key, HF, ExK, EqK, All>&
Hashtable_const_iterator<Val, Key, HF, ExK, EqK, All>::
operator++() {
    const Node* old = cur;
    cur = cur->next;
    if (!cur) {
        size_type bucket = ht->bkt_num(old->val);
        while (!cur && ++bucket < ht->buckets.size())
            cur = ht->buckets[bucket];
    }
    return *this;
}

template <class Val, class Key, class HF, class ExK, class EqK,
          class All>
inline Hashtable_const_iterator<Val, Key, HF, ExK, EqK, All>
Hashtable_const_iterator<Val, Key, HF, ExK, EqK, All>::
operator++(int) {
    const_iterator tmp = *this;
    ++*this;
    return tmp;
}

template <class Val, class Key, class HF, class Ex, class Eq, class All>
bool operator==(const hashtable<Val, Key, HF, Ex, Eq, All>& ht1,
                const hashtable<Val, Key, HF, Ex, Eq, All>& ht2) {
    typedef typename hashtable<Val, Key, HF, Ex, Eq, All>::Node Node;

    if (ht1.buckets.size() != ht2.buckets.size())
        return false;

    for (size_t n = 0; n < ht1.buckets.size(); ++n) {
        Node* cur1 = ht1.buckets[n];
        Node* cur2 = ht2.buckets[n];
        // Check same length of lists
        for (; cur1 && cur2;
             cur1 = cur1->next, cur2 = cur2->next) {}
        if (cur1 || cur2)
            return false;
        // Now check one's elements are in the other
        for (cur1 = ht1.buckets[n]; cur1;
             cur1 = cur1->next) {
            bool _foundcur1 = false;
            for (cur2 = ht2.buckets[n];
                 cur2; cur2 = cur2->next) {
                if (cur1->val == cur2->val) {
                    _foundcur1 = true;
                    break;
                }
            }
            if (!_foundcur1)
                return false;
        }
    }
    return true;
}

template <class Val, class Key, class HF, class Ex, class Eq, class All>
inline bool
operator!=(const hashtable<Val, Key, HF, Ex, Eq, All>& ht1,
           const hashtable<Val, Key, HF, Ex, Eq, All>& ht2) {
    return !(ht1 == ht2);
}

template <class Val, class Key, class HF, class Extract, class EqKey,
          class All>
inline void
swap(hashtable<Val, Key, HF, Extract, EqKey, All>& ht1,
     hashtable<Val, Key, HF, Extract, EqKey, All>& ht2) {
    ht1.swap(ht2);
}

template <class Val, class Key, class HF, class Ex, class Eq, class All>
mystl::pair<typename hashtable<Val, Key, HF, Ex, Eq, All>::iterator, bool>
hashtable<Val, Key, HF, Ex, Eq, All>::
    insert_unique_noresize(const value_type& obj) {
    const size_type n = bkt_num(obj);
    Node* first = buckets[n];

    for (Node* cur = first; cur; cur = cur->next)
        if (equals(get_key(cur->val), get_key(obj)))
            return mystl::pair<iterator, bool>(iterator(cur, this), false);

    Node* tmp = new_node(obj);
    tmp->next = first;
    buckets[n] = tmp;
    ++num_elements;
    return mystl::pair<iterator, bool>(iterator(tmp, this), true);
}

template <class Val, class Key, class HF, class Ex, class Eq, class All>
typename hashtable<Val, Key, HF, Ex, Eq, All>::iterator
hashtable<Val, Key, HF, Ex, Eq, All>::
    insert_equal_noresize(const value_type& obj) {
    const size_type n = bkt_num(obj);
    Node* first = buckets[n];

    for (Node* cur = first; cur; cur = cur->next)
        if (equals(get_key(cur->val), get_key(obj))) {
            Node* tmp = new_node(obj);
            tmp->next = cur->next;
            cur->next = tmp;
            ++num_elements;
            return iterator(tmp, this);
        }

    Node* tmp = new_node(obj);
    tmp->next = first;
    buckets[n] = tmp;
    ++num_elements;
    return iterator(tmp, this);
}

template <class Val, class Key, class HF, class Ex, class Eq, class All>
typename hashtable<Val, Key, HF, Ex, Eq, All>::reference
hashtable<Val, Key, HF, Ex, Eq, All>::
    find_or_insert(const value_type& obj) {
    resize(num_elements + 1);

    size_type n = bkt_num(obj);
    Node* first = buckets[n];

    for (Node* cur = first; cur; cur = cur->next)
        if (equals(get_key(cur->val), get_key(obj)))
            return cur->val;

    Node* tmp = new_node(obj);
    tmp->next = first;
    buckets[n] = tmp;
    ++num_elements;
    return tmp->val;
}

template <class Val, class Key, class HF, class Ex, class Eq, class All>
mystl::pair<typename hashtable<Val, Key, HF, Ex, Eq, All>::iterator,
            typename hashtable<Val, Key, HF, Ex, Eq, All>::iterator>
hashtable<Val, Key, HF, Ex, Eq, All>::
    equal_range(const key_type& key) {
    typedef mystl::pair<iterator, iterator> Pii;
    const size_type n = bkt_num_key(key);

    for (Node* first = buckets[n]; first;
         first = first->next)
        if (equals(get_key(first->val), key)) {
            for (Node* cur = first->next; cur;
                 cur = cur->next)
                if (!equals(get_key(cur->val), key))
                    return Pii(iterator(first, this), iterator(cur, this));
            for (size_type m = n + 1; m < buckets.size(); ++m)
                if (buckets[m])
                    return Pii(iterator(first, this),
                               iterator(buckets[m], this));
            return Pii(iterator(first, this), end());
        }
    return Pii(end(), end());
}

template <class Val, class Key, class HF, class Ex, class Eq, class All>
mystl::pair<typename hashtable<Val, Key, HF, Ex, Eq, All>::const_iterator,
            typename hashtable<Val, Key, HF, Ex, Eq, All>::const_iterator>
hashtable<Val, Key, HF, Ex, Eq, All>::
    equal_range(const key_type& key) const {
    typedef mystl::pair<const_iterator, const_iterator> Pii;
    const size_type n = bkt_num_key(key);

    for (const Node* first = buckets[n]; first;
         first = first->next) {
        if (equals(get_key(first->val), key)) {
            for (const Node* cur = first->next; cur;
                 cur = cur->next)
                if (!equals(get_key(cur->val), key))
                    return Pii(const_iterator(first, this),
                               const_iterator(cur, this));
            for (size_type m = n + 1; m < buckets.size(); ++m)
                if (buckets[m])
                    return Pii(const_iterator(first, this),
                               const_iterator(buckets[m], this));
            return Pii(const_iterator(first, this), end());
        }
    }
    return Pii(end(), end());
}

template <class Val, class Key, class HF, class Ex, class Eq, class All>
typename hashtable<Val, Key, HF, Ex, Eq, All>::size_type
hashtable<Val, Key, HF, Ex, Eq, All>::
    erase(const key_type& key) {
    const size_type n = bkt_num_key(key);
    Node* first = buckets[n];
    Node* saved_slot = 0;
    size_type erased = 0;

    if (first) {
        Node* cur = first;
        Node* next = cur->next;
        while (next) {
            if (equals(get_key(next->val), key)) {
                if (&get_key(next->val) != &key) {
                    cur->next = next->next;
                    delete_node(next);
                    next = cur->next;
                    ++erased;
                    --num_elements;
                } else {
                    saved_slot = cur;
                    cur = next;
                    next = cur->next;
                }
            } else {
                cur = next;
                next = cur->next;
            }
        }
        bool delete_first = equals(get_key(first->val), key);
        if (saved_slot) {
            next = saved_slot->next;
            saved_slot->next = next->next;
            delete_node(next);
            ++erased;
            --num_elements;
        }
        if (delete_first) {
            buckets[n] = first->next;
            delete_node(first);
            ++erased;
            --num_elements;
        }
    }
    return erased;
}

template <class Val, class Key, class HF, class Ex, class Eq, class All>
void hashtable<Val, Key, HF, Ex, Eq, All>::
    erase(const iterator& it) {
    Node* p = it.cur;
    if (p) {
        const size_type n = bkt_num(p->val);
        Node* cur = buckets[n];

        if (cur == p) {
            buckets[n] = cur->next;
            delete_node(cur);
            --num_elements;
        } else {
            Node* next = cur->next;
            while (next) {
                if (next == p) {
                    cur->next = next->next;
                    delete_node(next);
                    --num_elements;
                    break;
                } else {
                    cur = next;
                    next = cur->next;
                }
            }
        }
    }
}

template <class Val, class Key, class HF, class Ex, class Eq, class All>
void hashtable<Val, Key, HF, Ex, Eq, All>::
    erase(iterator first, iterator last) {
    size_type f_bucket = first.cur ? bkt_num(first.cur->val) : buckets.size();

    size_type l_bucket = last.cur ? bkt_num(last.cur->val) : buckets.size();

    if (first.cur == last.cur)
        return;
    else if (f_bucket == l_bucket)
        erase_bucket(f_bucket, first.cur, last.cur);
    else {
        erase_bucket(f_bucket, first.cur, 0);
        for (size_type n = f_bucket + 1; n < l_bucket; ++n)
            erase_bucket(n, 0);
        if (l_bucket != buckets.size())
            erase_bucket(l_bucket, last.cur);
    }
}

template <class Val, class Key, class HF, class Ex, class Eq, class All>
inline void
hashtable<Val, Key, HF, Ex, Eq, All>::
    erase(const_iterator first, const_iterator last) {
    erase(iterator(const_cast<Node*>(first.cur),
                   const_cast<hashtable*>(first.ht)),
          iterator(const_cast<Node*>(last.cur),
                   const_cast<hashtable*>(last.ht)));
}

template <class Val, class Key, class HF, class Ex, class Eq, class All>
inline void
hashtable<Val, Key, HF, Ex, Eq, All>::
    erase(const const_iterator& it) {
    erase(iterator(const_cast<Node*>(it.cur),
                   const_cast<hashtable*>(it.ht)));
}

template <class Val, class Key, class HF, class Ex, class Eq, class All>
void hashtable<Val, Key, HF, Ex, Eq, All>::
    resize(size_type num_elements_hint) {
    const size_type old_n = buckets.size();
    if (num_elements_hint > old_n) {
        const size_type n = next_size(num_elements_hint);
        if (n > old_n) {
            Vector_type tmp(n, (Node*)(0), buckets.get_allocator());
            try {
                for (size_type bucket = 0; bucket < old_n; ++bucket) {
                    Node* first = buckets[bucket];
                    while (first) {
                        size_type new_bucket = bkt_num(first->val,
                                                       n);
                        buckets[bucket] = first->next;
                        first->next = tmp[new_bucket];
                        tmp[new_bucket] = first;
                        first = buckets[bucket];
                    }
                }
                buckets.swap(tmp);
            } catch (...) {
                for (size_type bucket = 0; bucket < tmp.size();
                     ++bucket) {
                    while (tmp[bucket]) {
                        Node* next = tmp[bucket]->next;
                        delete_node(tmp[bucket]);
                        tmp[bucket] = next;
                    }
                }
                throw_exception_again;
            }
        }
    }
}

template <class Val, class Key, class HF, class Ex, class Eq, class All>
void hashtable<Val, Key, HF, Ex, Eq, All>::
    erase_bucket(const size_type n, Node* first, Node* last) {
    Node* cur = buckets[n];
    if (cur == first)
        erase_bucket(n, last);
    else {
        Node* next;
        for (next = cur->next;
             next != first;
             cur = next, next = cur->next)
            ;
        while (next != last) {
            cur->next = next->next;
            delete_node(next);
            next = cur->next;
            --num_elements;
        }
    }
}

template <class Val, class Key, class HF, class Ex, class Eq, class All>
void hashtable<Val, Key, HF, Ex, Eq, All>::
    erase_bucket(const size_type n, Node* last) {
    Node* cur = buckets[n];
    while (cur != last) {
        Node* next = cur->next;
        delete_node(cur);
        cur = next;
        buckets[n] = cur;
        --num_elements;
    }
}

template <class Val, class Key, class HF, class Ex, class Eq, class All>
void hashtable<Val, Key, HF, Ex, Eq, All>::
    clear() {
    if (num_elements == 0)
        return;

    for (size_type i = 0; i < buckets.size(); ++i) {
        Node* cur = buckets[i];
        while (cur != 0) {
            Node* next = cur->next;
            delete_node(cur);
            cur = next;
        }
        buckets[i] = 0;
    }
    num_elements = 0;
}

template <class Val, class Key, class HF, class Ex, class Eq, class All>
void hashtable<Val, Key, HF, Ex, Eq, All>::
    copy_from(const hashtable& ht) {
    buckets.clear();
    buckets.reserve(ht.buckets.size());
    buckets.insert(buckets.end(), ht.buckets.size(), (Node*)0);
    try {
        for (size_type i = 0; i < ht.buckets.size(); ++i) {
            const Node* cur = ht.buckets[i];
            if (cur) {
                Node* local_copy = new_node(cur->val);
                buckets[i] = local_copy;

                for (Node* next = cur->next;
                     next;
                     cur = next, next = cur->next) {
                    local_copy->next = new_node(next->val);
                    local_copy = local_copy->next;
                }
            }
        }
        num_elements = ht.num_elements;
    } catch (...) {
        clear();
        throw_exception_again;
    }
}

} // namespace mystl
#endif
