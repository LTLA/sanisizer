#include <gtest/gtest.h>

#include "sanisizer/arithmetic.hpp"

#include <cstdint>

// For brevity.
typedef std::int32_t i32;
typedef std::uint32_t u32;

TEST(Sum, Basic) {
    {
        static_assert(sanisizer::needs_sum_check<std::int64_t, std::int64_t, std::int64_t>());
        static_assert(sanisizer::needs_sum_check<std::int32_t, std::int64_t, std::int64_t>());
        static_assert(sanisizer::needs_sum_check<std::int64_t, std::int32_t, std::uint64_t>());
        static_assert(sanisizer::needs_sum_check<std::int64_t, std::uint32_t, std::int64_t>());

        static_assert(!sanisizer::needs_sum_check<std::int64_t, std::int32_t, std::int32_t>());
        static_assert(!sanisizer::needs_sum_check<std::int64_t, std::uint32_t, std::uint32_t>());
        static_assert(!sanisizer::needs_sum_check<std::int64_t, sanisizer::Attestation<std::int64_t, true, 10>, std::int32_t>());
        static_assert(!sanisizer::needs_sum_check<std::int64_t, std::int64_t, sanisizer::Attestation<std::int64_t, true, 0> >());
    }

    {
        EXPECT_EQ(sanisizer::sum<std::int64_t>((i32)5, (i32)20), 25);
        EXPECT_EQ(sanisizer::sum<std::int64_t>((u32)5, (i32)20), 25);
        EXPECT_EQ(sanisizer::sum<std::int64_t>((i32)5, (u32)20), 25);
        EXPECT_EQ(sanisizer::sum<std::int64_t>((u32)5, (u32)20), 25);

        EXPECT_EQ(sanisizer::sum<std::uint8_t>((i32)5, (i32)20), 25);
        EXPECT_EQ(sanisizer::sum<std::uint8_t>((u32)5, (i32)20), 25);
        EXPECT_EQ(sanisizer::sum<std::uint8_t>((i32)5, (u32)20), 25);
        EXPECT_EQ(sanisizer::sum<std::uint8_t>((u32)5, (u32)20), 25);

        EXPECT_EQ(sanisizer::sum<std::int8_t>(5, sanisizer::Attestation<i32, false, 1000>((i32)20)), 25);
        EXPECT_EQ(sanisizer::sum<std::int8_t>(5, sanisizer::Attestation<i32, false, 100>((i32)20)), 25);
        EXPECT_EQ(sanisizer::sum<std::int8_t>(sanisizer::Attestation<u32, true, 1000>((u32)20), 5), 25);
        EXPECT_EQ(sanisizer::sum<std::int8_t>(sanisizer::Attestation<u32, true, 100>((u32)20), 5), 25);
    }

    {
        bool failed = false;
        try {
            sanisizer::sum<std::int8_t>(5, 255);
        } catch (std::overflow_error& e) {
            failed = true;
        }
        EXPECT_TRUE(failed);

        failed = false;
        try {
            sanisizer::sum<std::int8_t>(5, -1);
        } catch (std::out_of_range& e) {
            failed = true;
        }
        EXPECT_TRUE(failed);
    }

    // Still works with additional arguments.
    EXPECT_EQ(sanisizer::sum<std::int64_t>(5, 20, 50), 75);
    {
        bool failed = false;
        try {
            sanisizer::sum<std::int8_t>(200, 50, 6);
        } catch (std::overflow_error& e) {
            failed = true;
        }
        EXPECT_TRUE(failed);

        failed = false;
        try {
            sanisizer::sum<std::int8_t>(6, 50, 200);
        } catch (std::overflow_error& e) {
            failed = true;
        }
        EXPECT_TRUE(failed);
    }
}

TEST(Sum, Unsafe) {
    EXPECT_EQ(sanisizer::sum_unsafe<std::uint8_t>((i32)5, (i32)20), 25);
    EXPECT_EQ(sanisizer::sum_unsafe<std::uint8_t>((u32)5, (i32)20), 25);
    EXPECT_EQ(sanisizer::sum_unsafe<std::uint8_t>((i32)5, (u32)20), 25);
    EXPECT_EQ(sanisizer::sum_unsafe<std::uint8_t>((u32)5, (u32)20, (u32)100), 125);

    EXPECT_EQ(sanisizer::sum_unsafe<std::int64_t>((i32)5, (i32)20), 25);
    EXPECT_EQ(sanisizer::sum_unsafe<std::int64_t>((u32)5, (i32)20), 25);
    EXPECT_EQ(sanisizer::sum_unsafe<std::int64_t>((i32)5, (u32)20), 25);
    EXPECT_EQ(sanisizer::sum_unsafe<std::int64_t>((u32)5, (u32)20, (u32)100), 125);
}

TEST(Product, Basic) {
    {
        static_assert(sanisizer::needs_product_check<std::int64_t, std::int64_t, std::int64_t>());
        static_assert(sanisizer::needs_product_check<std::int32_t, std::int64_t, std::int64_t>());
        static_assert(sanisizer::needs_product_check<std::int32_t, std::int32_t, std::uint32_t>());
        static_assert(sanisizer::needs_product_check<std::int32_t, std::uint32_t, std::int32_t>());
        static_assert(sanisizer::needs_product_check<std::int64_t, std::uint32_t, std::uint32_t>());

        static_assert(!sanisizer::needs_product_check<std::int64_t, std::int32_t, std::int32_t>());
        static_assert(!sanisizer::needs_product_check<std::int64_t, std::int8_t, std::uint32_t>());
        static_assert(!sanisizer::needs_product_check<std::int64_t, std::int32_t, std::uint32_t>());
        static_assert(!sanisizer::needs_product_check<std::int64_t, std::uint32_t, std::int32_t>());

        static_assert(!sanisizer::needs_product_check<std::int64_t, sanisizer::Attestation<std::int64_t, true, 100>, std::uint32_t>());
        static_assert(!sanisizer::needs_product_check<std::int64_t, sanisizer::Attestation<std::int64_t, true, 0>, std::int64_t>());
        static_assert(!sanisizer::needs_product_check<std::int64_t, std::int32_t, sanisizer::Attestation<std::int64_t, true, 100> >());
        static_assert(!sanisizer::needs_product_check<std::int64_t, std::int64_t, sanisizer::Attestation<std::int64_t, true, 0> >());
    }

    {
        EXPECT_EQ(sanisizer::product<std::int64_t>((i32)5, (i32)20), 100);
        EXPECT_EQ(sanisizer::product<std::int64_t>((i32)5, (u32)20), 100);
        EXPECT_EQ(sanisizer::product<std::int64_t>((u32)5, (i32)20), 100);
        EXPECT_EQ(sanisizer::product<std::int64_t>((u32)5, (u32)20), 100);

        EXPECT_EQ(sanisizer::product<std::uint8_t>((i32)5, (i32)20), 100);
        EXPECT_EQ(sanisizer::product<std::uint8_t>((i32)5, (u32)20), 100);
        EXPECT_EQ(sanisizer::product<std::uint8_t>((u32)5, (i32)20), 100);
        EXPECT_EQ(sanisizer::product<std::uint8_t>((u32)5, (u32)20), 100);

        EXPECT_EQ(sanisizer::product<std::uint8_t>((u32)50, (u32)0), 0);
        EXPECT_EQ(sanisizer::product<std::uint8_t>((u32)0, (u32)50), 0);
        EXPECT_EQ(sanisizer::product<std::int64_t>((u32)50, (u32)0), 0);
        EXPECT_EQ(sanisizer::product<std::int64_t>((u32)0, (u32)50), 0);

        EXPECT_EQ(sanisizer::product<std::int8_t>(5, sanisizer::Attestation<u32, true, 1000>((u32)20)), 100);
        EXPECT_EQ(sanisizer::product<std::int8_t>(5, sanisizer::Attestation<u32, true, 100>((u32)20)), 100);
        EXPECT_EQ(sanisizer::product<std::int8_t>(sanisizer::Attestation<i32, true, 1000>((i32)20), 5), 100);
        EXPECT_EQ(sanisizer::product<std::int8_t>(sanisizer::Attestation<i32, true, 100>((i32)20), 5), 100);
    }

    {
        bool failed = false;
        try {
            sanisizer::product<std::int8_t>(5, 255);
        } catch (std::overflow_error& e) {
            failed = true;
        }
        EXPECT_TRUE(failed);

        failed = false;
        try {
            sanisizer::product<std::int8_t>(0, 1000);
        } catch (std::overflow_error& e) {
            failed = true;
        }
        EXPECT_TRUE(failed);

        failed = false;
        try {
            sanisizer::product<std::int8_t>(-10, -10);
        } catch (std::out_of_range& e) {
            failed = true;
        }
        EXPECT_TRUE(failed);
    }

    // Still works with additional arguments.
    EXPECT_EQ(sanisizer::product<std::int64_t>(5, 20, 50), 5000);
    {
        bool failed = false;
        try {
            sanisizer::product<std::int8_t>(10, 10, 10);
        } catch (std::overflow_error& e) {
            failed = true;
        }
        EXPECT_TRUE(failed);

        failed = false;
        try {
            sanisizer::product<std::int8_t>(0, 0, 1000);
        } catch (std::overflow_error& e) {
            failed = true;
        }
        EXPECT_TRUE(failed);
    }
}

TEST(Product, Unsafe) {
    EXPECT_EQ(sanisizer::product_unsafe<std::uint8_t>((i32)5, (i32)20), 100);
    EXPECT_EQ(sanisizer::product_unsafe<std::uint8_t>((u32)5, (i32)20), 100);
    EXPECT_EQ(sanisizer::product_unsafe<std::uint8_t>((i32)5, (u32)20), 100);
    EXPECT_EQ(sanisizer::product_unsafe<std::uint8_t>((u32)5, (u32)20, (u32)2), 200);

    EXPECT_EQ(sanisizer::product_unsafe<std::int64_t>((i32)5, (i32)20), 100);
    EXPECT_EQ(sanisizer::product_unsafe<std::int64_t>((u32)5, (i32)20), 100);
    EXPECT_EQ(sanisizer::product_unsafe<std::int64_t>((i32)5, (u32)20), 100);
    EXPECT_EQ(sanisizer::product_unsafe<std::int64_t>((u32)5, (u32)20, (u32)100), 10000);
}
