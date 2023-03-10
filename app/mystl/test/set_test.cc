#ifndef MYSTL_SET_TEST_H_
#define MYSTL_SET_TEST_H_

#include "set.h"
#include "htest.h"
#include "htest_utils.h"

#include <set>
#include <string>
#include <array>

namespace mystl {
namespace test {
namespace set_test {

bool fncomp(int lhs, int rhs) {
    return lhs < rhs;
}

struct classcomp {
    bool operator()(const int& lhs, const int& rhs) const {
        return lhs < rhs;
    }
};

TEST(set) {
    {
        mystl::set<int> first; // empty set of ints
        int myints[] = {10, 20, 30, 40, 50};
        mystl::set<int> second(myints, myints + 5);           // range
        mystl::set<int> third(second);                        // a copy of second
        mystl::set<int> fourth(second.begin(), second.end()); // iterator ctor.

        mystl::set<int, classcomp> fifth; // class as Compare

        bool (*fn_pt)(int, int) = fncomp;
        mystl::set<int, bool (*)(int, int)> sixth(fn_pt); // function pointer as Compare
    }

    {
        mystl::set<int> myset;
        mystl::set<int>::iterator it;
        mystl::pair<mystl::set<int>::iterator, bool> ret;

        // set some initial values:
        for (int i = 1; i <= 5; ++i) myset.insert(i * 10); // set: 10 20 30 40 50

        ret = myset.insert(20); // no new element inserted

        if (ret.second == false) it = ret.first; // "it" now points to element 20

        myset.insert(it, 25); // max efficiency inserting
        myset.insert(it, 24); // max efficiency inserting
        myset.insert(it, 26); // no max efficiency inserting

        int myints[] = {5, 10, 15}; // 10 already in set, not inserted
        myset.insert(myints, myints + 3);

        int expect[] = {5, 10, 15, 20, 24, 25, 26, 30, 40, 50};
        std::set<int> s1(expect, expect + sizeof(expect) / sizeof(expect[0]));
        EXPECT_TRUE(htest::ContainerEqual(myset, s1));
    }
}
}
}
} // namespace mystl::test::set_test
#endif // MYSTL_SET_TEST_H_
