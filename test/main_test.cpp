#include <gtest/gtest.h>
#include <iostream>

TEST(TestSuite, Startup) {
    std::cout << " Beginning test suite " << std::endl;
    // Sanity checks
    ASSERT_TRUE(true);
    ASSERT_FALSE(false);
}

