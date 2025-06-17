#include <gtest/gtest.h>

#include "sanisizer/arithmetic.hpp"

#include <cstdint>

TEST(Sum, Basic) {
    {
        bool check = sanisizer::needs_sum_check<std::int64_t, std::int64_t, std::int64_t>();
        EXPECT_TRUE(check);
    }
    {
        bool check = sanisizer::needs_sum_check<std::int32_t, std::int64_t, std::int64_t>();
        EXPECT_TRUE(check);
    }
    {
        bool check = sanisizer::needs_sum_check<std::int64_t, std::int32_t, std::uint32_t>();
        EXPECT_FALSE(check);
    }
    {
        bool check = sanisizer::needs_sum_check<std::int64_t, std::int32_t, std::uint64_t>();
        EXPECT_TRUE(check);
    }

    EXPECT_EQ(sanisizer::sum<std::int64_t>(5, 20), 25);
    EXPECT_EQ(sanisizer::sum<std::int64_t>(5u, 20u), 25);
    EXPECT_EQ(sanisizer::sum<std::int64_t>(5u, 20u), 25);

    EXPECT_EQ(sanisizer::sum<std::int8_t>(5, 20), 25);
    EXPECT_EQ(sanisizer::sum<std::int8_t>(5u, 20u), 25);
    EXPECT_EQ(sanisizer::sum<std::int8_t>(5u, 20u), 25);

    bool failed = false;
    try {
        sanisizer::sum<std::int8_t>(5, 255);
    } catch (sanisizer::OverflowError& e) {
        failed = true;
    }
    EXPECT_TRUE(failed);
}

TEST(Sum, Unsafe) {
    EXPECT_EQ(sanisizer::sum_unsafe<std::int64_t>(5, 20), 25);
    EXPECT_EQ(sanisizer::sum_unsafe<std::int64_t>(5u, 20u), 25);
    EXPECT_EQ(sanisizer::sum_unsafe<std::int64_t>(5u, 20u), 25);
}

TEST(Product, Basic) {
    {
        bool check = sanisizer::needs_product_check<std::int64_t, std::int64_t, std::int64_t>();
        EXPECT_TRUE(check);
    }
    {
        bool check = sanisizer::needs_product_check<std::int32_t, std::int64_t, std::int64_t>();
        EXPECT_TRUE(check);
    }
    {
        bool check = sanisizer::needs_product_check<std::int64_t, std::int32_t, std::uint32_t>();
        EXPECT_FALSE(check);
    }
    {
        bool check = sanisizer::needs_product_check<std::int64_t, std::int8_t, std::uint8_t>();
        EXPECT_FALSE(check);
    }
    {
        bool check = sanisizer::needs_product_check<std::int64_t, std::int32_t, std::uint64_t>();
        EXPECT_TRUE(check);
    }

    EXPECT_EQ(sanisizer::product<std::int64_t>(5, 20), 100);
    EXPECT_EQ(sanisizer::product<std::int64_t>(5u, 20u), 100);
    EXPECT_EQ(sanisizer::product<std::int64_t>(5u, 20u), 100);

    EXPECT_EQ(sanisizer::product<std::int8_t>(5, 20), 100);
    EXPECT_EQ(sanisizer::product<std::int8_t>(5u, 20u), 100);
    EXPECT_EQ(sanisizer::product<std::int8_t>(5u, 20u), 100);
    EXPECT_EQ(sanisizer::product<std::int8_t>(0u, 20u), 0);

    bool failed = false;
    try {
        sanisizer::product<std::int8_t>(5, 255);
    } catch (sanisizer::OverflowError& e) {
        failed = true;
    }
    EXPECT_TRUE(failed);

    failed = false;
    try {
        sanisizer::product<std::int8_t>(0, 1000);
    } catch (sanisizer::OverflowError& e) {
        failed = true;
    }
    EXPECT_TRUE(failed);
}

TEST(Product, Unsafe) {
    EXPECT_EQ(sanisizer::product_unsafe<std::int64_t>(5, 20), 100);
    EXPECT_EQ(sanisizer::product_unsafe<std::int64_t>(5u, 20u), 100);
    EXPECT_EQ(sanisizer::product_unsafe<std::int64_t>(5u, 20u), 100);
}
