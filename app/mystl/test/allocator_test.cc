﻿#ifndef MYTINYSTL_ALLOCATOR_TEST_H_
#define MYTINYSTL_ALLOCATOR_TEST_H_

#include <memory>
#include <iostream>
#include <string>
 

#include "allocator.h"
#include "unit_test.h"

using namespace unit_test;

namespace MyStl {
namespace test {
namespace allocator_test {

TEST(allocator) {
    {
        // default allocator for ints
        MyStl::allocator<int> alloc;

        // demonstrating the few directly usable members
        EXPECT_TRUE((std::is_same<int, decltype(alloc)::value_type>::value));
        int* p = alloc.allocate(1); // space for one int
        alloc.deallocate(p, 1);     // and it is gone

        // Even those can be used through traits though, so no need
        using traits_t = std::allocator_traits<decltype(alloc)>; // The matching trait
        p = traits_t::allocate(alloc, 1);
        traits_t::construct(alloc, p, 7); // construct the int
        std::cout << *p << '\n';
        traits_t::deallocate(alloc, p, 1); // deallocate space for one int
    }

    {
        // default allocator for strings
        MyStl::allocator<std::string> alloc;
        // matching traits
        using traits_t = std::allocator_traits<decltype(alloc)>;

        // Rebinding the allocator using the trait for strings gets the same type
        traits_t::rebind_alloc<std::string> alloc_ = alloc;

        std::string* p = traits_t::allocate(alloc, 2); // space for 2 strings

        traits_t::construct(alloc, p, "foo");
        traits_t::construct(alloc, p + 1, "bar");

        EXPECT_EQ(p[0], "foo1");
        EXPECT_EQ(p[1], "bar");

        traits_t::destroy(alloc, p + 1);
        traits_t::destroy(alloc, p);
        traits_t::deallocate(alloc, p, 2);
    }
}

}
}
} // namespace MyStl::test::allocator_test
#endif // !MYTINYSTL_ALLOCATOR_TEST_H_
