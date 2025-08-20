#include <gtest/gtest.h>

#include "sanisizer/comparisons.hpp"

TEST(Comparisons, Equal) {
    EXPECT_TRUE(sanisizer::is_equal(1, 1));
    EXPECT_FALSE(sanisizer::is_equal(1, 0));
    EXPECT_TRUE(sanisizer::is_equal(1u, 1u));
    EXPECT_FALSE(sanisizer::is_equal(1u, 0u));
    EXPECT_TRUE(sanisizer::is_equal(1, 1u));
    EXPECT_FALSE(sanisizer::is_equal(1u, 0));
}

TEST(Comparisons, GreaterThan) {
    EXPECT_FALSE(sanisizer::is_greater_than(1, 1));
    EXPECT_TRUE(sanisizer::is_greater_than(1, 0));
    EXPECT_FALSE(sanisizer::is_greater_than(1u, 1u));
    EXPECT_TRUE(sanisizer::is_greater_than(1u, 0u));
    EXPECT_FALSE(sanisizer::is_greater_than(1, 1u));
    EXPECT_TRUE(sanisizer::is_greater_than(1u, 0));
}

TEST(Comparisons, LessThanOrEqual) {
    EXPECT_TRUE(sanisizer::is_less_than_or_equal(1, 1));
    EXPECT_TRUE(sanisizer::is_less_than_or_equal(0, 1));
    EXPECT_FALSE(sanisizer::is_less_than_or_equal(1, 0));
    EXPECT_TRUE(sanisizer::is_less_than_or_equal(1u, 1u));
    EXPECT_TRUE(sanisizer::is_less_than_or_equal(0u, 1u));
    EXPECT_FALSE(sanisizer::is_less_than_or_equal(1u, 0u));
    EXPECT_TRUE(sanisizer::is_less_than_or_equal(1, 1u));
    EXPECT_TRUE(sanisizer::is_less_than_or_equal(0, 1u));
    EXPECT_FALSE(sanisizer::is_less_than_or_equal(1u, 0));
}

TEST(Comparisons, LessThan) {
    EXPECT_TRUE(sanisizer::is_less_than(0, 1));
    EXPECT_FALSE(sanisizer::is_less_than(1, 1));
    EXPECT_TRUE(sanisizer::is_less_than(0u, 1u));
    EXPECT_FALSE(sanisizer::is_less_than(1u, 1u));
    EXPECT_TRUE(sanisizer::is_less_than(0u, 1));
    EXPECT_FALSE(sanisizer::is_less_than(1, 1u));
}

TEST(Comparisons, GreaterThanOrEqual) {
    EXPECT_TRUE(sanisizer::is_greater_than_or_equal(1, 1));
    EXPECT_TRUE(sanisizer::is_greater_than_or_equal(1, 0));
    EXPECT_FALSE(sanisizer::is_greater_than_or_equal(0, 1));
    EXPECT_TRUE(sanisizer::is_greater_than_or_equal(1u, 1u));
    EXPECT_TRUE(sanisizer::is_greater_than_or_equal(1u, 0u));
    EXPECT_FALSE(sanisizer::is_greater_than_or_equal(0u, 1u));
    EXPECT_TRUE(sanisizer::is_greater_than_or_equal(1, 1u));
    EXPECT_TRUE(sanisizer::is_greater_than_or_equal(1, 0u));
    EXPECT_FALSE(sanisizer::is_greater_than_or_equal(0u, 1));
}

TEST(Comparisons, Min) {
    auto x = sanisizer::min(static_cast<unsigned char>(255), 100);
    static_assert(std::is_same<decltype(x), unsigned char>::value);
    EXPECT_EQ(x, 100u);
    static_assert(sanisizer::min(static_cast<unsigned char>(255), 100) == 100u); // works at compile-time.

    auto x2 = sanisizer::min(static_cast<unsigned char>(255), 1000);
    static_assert(std::is_same<decltype(x2), unsigned char>::value);
    EXPECT_EQ(x2, 255u);
    static_assert(sanisizer::min(static_cast<unsigned char>(255), 1000) == 255u);

    auto x3 = sanisizer::min(500u, 1000);
    static_assert(std::is_same<decltype(x3), int>::value);
    EXPECT_EQ(x3, 500);
    static_assert(sanisizer::min(500u, 1000) == 500);

    auto x4 = sanisizer::min(10000u, 1000);
    static_assert(std::is_same<decltype(x4), int>::value);
    EXPECT_EQ(x4, 1000);
    static_assert(sanisizer::min(10000u, 1000) == 1000);
}

TEST(Comparisons, Max) {
    auto x = sanisizer::max(static_cast<unsigned char>(255), 100);
    static_assert(std::is_same<decltype(x), int>::value);
    EXPECT_EQ(x, 255);
    static_assert(sanisizer::max(static_cast<unsigned char>(255), 100) == 255); // works at compile-time.

    auto x2 = sanisizer::max(static_cast<unsigned char>(255), 1000);
    static_assert(std::is_same<decltype(x2), int>::value);
    EXPECT_EQ(x2, 1000);
    static_assert(sanisizer::max(static_cast<unsigned char>(255), 1000) == 1000);

    auto x3 = sanisizer::max(500u, 1000);
    static_assert(std::is_same<decltype(x3), unsigned int>::value);
    EXPECT_EQ(x3, 1000u);
    static_assert(sanisizer::max(500u, 1000) == 1000u);

    auto x4 = sanisizer::max(10000u, 1000);
    static_assert(std::is_same<decltype(x4), unsigned int>::value);
    EXPECT_EQ(x4, 10000u);
    static_assert(sanisizer::max(10000u, 1000) == 10000u);
}
