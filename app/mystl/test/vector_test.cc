#ifndef MYSTL_VECTOR_TEST_H_
#define MYSTL_VECTOR_TEST_H_

#include "vector.h"
#include "htest.h"
#include "htest_utils.h"

#include <vector>
#include <string>
#include <array>

namespace mystl {
namespace test {
namespace vector_test {

TEST(vector_testCase1) {
    std::vector<std::string> v1(10, "zxh");
    mystl::vector<std::string> v2(10, "zxh");
    EXPECT_TRUE(htest::ContainerEqual(v1, v2));

    std::vector<std::string> v3(10);
    mystl::vector<std::string> v4(10);
    EXPECT_TRUE(htest::ContainerEqual(v3, v4));

    std::array<std::string, 3> arr = {"abc", "def", "ghi"};
    std::vector<std::string> v5(std::begin(arr), std::end(arr));
    mystl::vector<std::string> v6(std::begin(arr), std::end(arr));
    EXPECT_TRUE(htest::ContainerEqual(v5, v6));
}

TEST(vector_testCase2) {
    std::vector<int> temp1(10, 0);
    mystl::vector<int> temp2(10, 0);

    auto v1(temp1);
    auto v2(temp2);
    EXPECT_TRUE(htest::ContainerEqual(v1, v2));

    auto v3(std::move(temp1));
    auto v4(std::move(temp2));
    EXPECT_TRUE(htest::ContainerEqual(v3, v4));

    auto v5 = v1;
    auto v6 = v2;
    EXPECT_TRUE(htest::ContainerEqual(v5, v6));

    auto v7 = std::move(v3);
    auto v8 = std::move(v4);
    EXPECT_TRUE(htest::ContainerEqual(v7, v8));
}

TEST(vector_testCase3) {
    mystl::vector<int> v1, v2;
    for (int i = 0; i != 100; ++i) {
        v1.push_back(i);
        v2.push_back(i);
    }

    EXPECT_TRUE(v1 == v2);
    EXPECT_FALSE(v1 != v2);
}

TEST(vector_testCase4) {
    mystl::vector<int> myvector;
    for (int i = 1; i <= 5; i++) myvector.push_back(i);

    auto i = 1;
    for (mystl::vector<int>::iterator it = myvector.begin(); it != myvector.end(); ++it, ++i) {
        EXPECT_EQ(*it, i);
    }

    i = 1;
    for (mystl::vector<int>::const_iterator it = myvector.cbegin(); it != myvector.cend(); ++it, ++i) {
        EXPECT_EQ(*it, i);
    }
}

TEST(vector_testCase5) {
    mystl::vector<int> myvector(5); // 5 default-constructed ints
    int i = 0;
    mystl::vector<int>::reverse_iterator rit = myvector.rbegin();
    for (; rit != myvector.rend(); ++rit)
        *rit = ++i;

    i = 5;
    for (mystl::vector<int>::iterator it = myvector.begin(); it != myvector.end(); ++it, --i) {
        EXPECT_EQ(*it, i);
    }

    i = 1;
    for (mystl::vector<int>::reverse_iterator it = myvector.rbegin(); it != myvector.rend(); ++it, ++i) {
        EXPECT_EQ(*it, i);
    }
}

TEST(vector_testCase6) {
    mystl::vector<int> v(11, 0);
    EXPECT_EQ(v.size(), 11);

    v.resize(5);
    EXPECT_EQ(v.size(), 5);

    v.resize(20);
    EXPECT_EQ(v.size(), 20);
}

TEST(vector_testCase7) {
    mystl::vector<int> v;
    v.reserve(20);
    EXPECT_EQ(v.capacity(), 20);
}

TEST(vector_testCase8) {
    std::vector<int> v1(10);
    mystl::vector<int> v2(10);
    for (unsigned i = 0; i < 10; i++) {
        v1[i] = i;
        v2[i] = i;
    }
    EXPECT_TRUE(htest::ContainerEqual(v1, v2));

    v1.front() = 99;
    v2.front() = 99;
    v1.back() = 100;
    v2.back() = 100;

    EXPECT_TRUE(htest::ContainerEqual(v1, v2));
}

TEST(vector_testCase9) {
    std::vector<int> v1(5);
    mystl::vector<int> v2(5);

    auto p1 = v1.data();
    auto p2 = v2.data();
    *p1 = 10;
    ++p1;
    *p1 = 20;
    p1[2] = 100;
    *p2 = 10;
    ++p2;
    *p2 = 20;
    p2[2] = 100;

    EXPECT_TRUE(htest::ContainerEqual(v1, v2));
}

TEST(vector_testCase10) {
    mystl::vector<int> foo(3, 100); // three ints with a value of 100
    mystl::vector<int> bar(2, 200); // five ints with a value of 200

    std::vector<int> v1{100, 100, 100};
    std::vector<int> v2{200, 200};
    EXPECT_TRUE(htest::ContainerEqual(foo, v1));
    EXPECT_TRUE(htest::ContainerEqual(bar, v2));

    foo.swap(bar);
    EXPECT_TRUE(htest::ContainerEqual(bar, v1));
    EXPECT_TRUE(htest::ContainerEqual(foo, v2));
}

TEST(vector_testCase11) {
    std::vector<std::string> v1;
    mystl::vector<std::string> v2;

    v1.push_back("hello ");
    v1.push_back("world");
    v2.push_back("hello ");
    v2.push_back("world");
    EXPECT_TRUE(htest::ContainerEqual(v1, v2));

    v1.pop_back();
    v2.pop_back();
    EXPECT_TRUE(htest::ContainerEqual(v1, v2));
}

TEST(vector_testCase12) {
    std::vector<int> v1;
    mystl::vector<int> v2;

    v1.insert(v1.begin(), 0);
    v2.insert(v2.begin(), 0);
    EXPECT_TRUE(htest::ContainerEqual(v1, v2));

    v1.insert(v1.end(), 1);
    v2.insert(v2.end(), 1);
    EXPECT_TRUE(htest::ContainerEqual(v1, v2));

    v1.insert(v1.begin() + v1.size() / 2, 10, 0);
    v2.insert(v2.begin() + v2.size() / 2, 10, 0);
    EXPECT_TRUE(htest::ContainerEqual(v1, v2));

    int arr[] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
    v1.insert(v1.end(), std::begin(arr), std::end(arr));
    v2.insert(v2.end(), std::begin(arr), std::end(arr));
    EXPECT_TRUE(htest::ContainerEqual(v1, v2));
}

TEST(vector_testCase13) {
    std::vector<int> v1;
    mystl::vector<int> v2;
    for (int i = 1; i <= 10; i++) {
        v1.push_back(i);
        v2.push_back(i);
    }
    v1.erase(v1.begin() + 5);
    v2.erase(v2.begin() + 5);
    EXPECT_TRUE(htest::ContainerEqual(v1, v2));

    v1.erase(v1.begin(), v1.begin() + 3);
    v2.erase(v2.begin(), v2.begin() + 3);
    EXPECT_TRUE(htest::ContainerEqual(v1, v2));
}

TEST(vector_testCase14) {
    mystl::vector<int> foo(3, 100);
    mystl::vector<int> bar(2, 200);

    EXPECT_TRUE(!(foo == bar));
    EXPECT_TRUE(foo != bar);
}

TEST(vector_testCase15) {
    mystl::vector<int> myvector(100);
    EXPECT_EQ(100, myvector.capacity());

    myvector.resize(10);
    EXPECT_EQ(100, myvector.capacity());

    myvector.shrink_to_fit();
    EXPECT_EQ(10, myvector.capacity());
}

TEST(vector_testCase16) {
    mystl::vector<int> first;
    mystl::vector<int> second;
    mystl::vector<int> third;
    std::vector<int> first1;
    std::vector<int> second1;
    std::vector<int> third1;

    first.assign(7, 100); // 7 ints with a value of 100
    first1.assign(7, 100);

    mystl::vector<int>::iterator it;
    std::vector<int>::iterator it1;
    it = first.begin() + 1;
    it1 = first1.begin() + 1;

    second.assign(it, first.end() - 1); // the 5 central values of first
    second1.assign(it1, first1.end() - 1);

    int myints[] = {1776, 7, 4};
    third.assign(myints, myints + 3); // assigning from array.
    third1.assign(myints, myints + 3);

    EXPECT_TRUE(htest::ContainerEqual(first, first1));
    EXPECT_TRUE(htest::ContainerEqual(second, second1));
    EXPECT_TRUE(htest::ContainerEqual(third, third1));
}
}
}
} // namespace mystl::test::vector_test
#endif // MYSTL_VECTOR_TEST_H_
