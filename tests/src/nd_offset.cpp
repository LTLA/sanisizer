#include <gtest/gtest.h>

#include "sanisizer/nd_offset.hpp"

#include <cstdint>

TEST(NdOffset, Basic) {
    EXPECT_EQ(sanisizer::nd_offset<int>(1, 2, 3), 7);
    EXPECT_EQ(sanisizer::nd_offset<int>(2, 0, 5), 2);
    EXPECT_EQ(sanisizer::nd_offset<int>(20, 10, 5), 70);
    EXPECT_EQ(sanisizer::nd_offset<int>(1, 2, 3, 4, 5), ((4 * 5) + 3) * 2 + 1);
    EXPECT_EQ(sanisizer::nd_offset<int>(1, 2, 3, 4, 5, 6, 7), (((6 * 7) + 5) * 4 + 3) * 2 + 1);

    // Works at compile time.
    static_assert(sanisizer::nd_offset<int>(1, 2, 3, 4, 5) == ((4 * 5) + 3) * 2 + 1);
}
