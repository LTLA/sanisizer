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
