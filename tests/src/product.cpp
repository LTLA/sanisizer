#include <gtest/gtest.h>

#include "sanisizer/product.hpp"

#include <cstdint>

// For brevity.
typedef std::int32_t i32;
typedef std::uint32_t u32;

TEST(Product, Check) {
    static_assert(sanisizer::needs_product_check<std::int64_t, std::int64_t, std::int64_t>());
    static_assert(sanisizer::needs_product_check<std::int32_t, std::int64_t, std::int64_t>());
    static_assert(sanisizer::needs_product_check<std::int32_t, std::int32_t, std::uint32_t>());
    static_assert(sanisizer::needs_product_check<std::int32_t, std::uint32_t, std::int32_t>());
    static_assert(sanisizer::needs_product_check<std::int64_t, std::uint32_t, std::uint32_t>());

    static_assert(!sanisizer::needs_product_check<std::int64_t, std::int32_t, std::int32_t>());
    static_assert(!sanisizer::needs_product_check<std::int64_t, std::int8_t, std::uint32_t>());
    static_assert(!sanisizer::needs_product_check<std::int64_t, std::int32_t, std::uint32_t>());
    static_assert(!sanisizer::needs_product_check<std::int64_t, std::uint32_t, std::int32_t>());

    static_assert(!sanisizer::needs_product_check<std::int64_t, sanisizer::Attestation<std::int64_t, 100>, std::uint32_t>());
    static_assert(!sanisizer::needs_product_check<std::int64_t, sanisizer::Attestation<std::int64_t, 0>, std::int64_t>());
    static_assert(!sanisizer::needs_product_check<std::int64_t, std::int32_t, sanisizer::Attestation<std::int64_t, 100> >());
    static_assert(!sanisizer::needs_product_check<std::int64_t, std::int64_t, sanisizer::Attestation<std::int64_t, 0> >());
}

TEST(Product, Integer) {
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

    // Works at compile-time.
    static_assert(sanisizer::product<std::int8_t>((i32)5, (i32)20) == 100);
    static_assert(sanisizer::product<std::int64_t>((i32)5, (i32)20) == 100);
}

TEST(Product, Bounded) {
    EXPECT_EQ(sanisizer::product<std::int8_t>(5, sanisizer::Attestation<u32, 1000>((u32)20)), 100);
    EXPECT_EQ(sanisizer::product<std::int64_t>(5, sanisizer::Attestation<u32, 100>((u32)20)), 100);
    EXPECT_EQ(sanisizer::product<std::int8_t>(sanisizer::Attestation<i32, 1000>((i32)20), 5), 100);
    EXPECT_EQ(sanisizer::product<std::int64_t>(sanisizer::Attestation<i32, 100>((i32)20), 5), 100);

    EXPECT_EQ(sanisizer::product<std::uint8_t>(sanisizer::Attestation<u32, 100>((u32)50), (u32)0), 0);
    EXPECT_EQ(sanisizer::product<std::uint64_t>(sanisizer::Attestation<u32, 100>((u32)0), (u32)50), 0);
    EXPECT_EQ(sanisizer::product<std::uint8_t>(50, sanisizer::Attestation<u32, 100>((u32)0)), 0);
    EXPECT_EQ(sanisizer::product<std::uint64_t>(0, sanisizer::Attestation<u32, 100>((u32)50)), 0);

    // Works at compile-time.
    static_assert(sanisizer::product<std::int8_t>(sanisizer::Attestation<i32, 20>((i32)20), sanisizer::Attestation<u32, 10>(5)) == 100);
    static_assert(sanisizer::product<std::int64_t>(sanisizer::Attestation<i32, 20>((i32)20), sanisizer::Attestation<u32, 10>(5)) == 100);
}

TEST(Product, Constant) {
    EXPECT_EQ(sanisizer::product<std::int8_t>(5, sanisizer::Constant<u32, 20>()), 100);
    EXPECT_EQ(sanisizer::product<std::int64_t>(5, sanisizer::Constant<u32, 20>()), 100);
    EXPECT_EQ(sanisizer::product<std::int8_t>(sanisizer::Constant<i32, 20>(), 5), 100);
    EXPECT_EQ(sanisizer::product<std::int64_t>(sanisizer::Constant<i32, 20>(), 5), 100);

    EXPECT_EQ(sanisizer::product<std::uint8_t>(sanisizer::Constant<u32, 50>(), (u32)0), 0);
    EXPECT_EQ(sanisizer::product<std::uint64_t>(sanisizer::Constant<u32, 0>(), (u32)50), 0);
    EXPECT_EQ(sanisizer::product<std::uint8_t>(50, sanisizer::Constant<u32, 0>()), 0);
    EXPECT_EQ(sanisizer::product<std::uint64_t>(0, sanisizer::Constant<u32, 100>()), 0);

    // Works at compile-time.
    static_assert(sanisizer::product<std::int8_t>(sanisizer::Constant<i32, 20>(), sanisizer::Constant<u32, 5>()) == 100);
    static_assert(sanisizer::product<std::int64_t>(sanisizer::Constant<i32, 20>(), sanisizer::Constant<u32, 5>()) == 100);
}

TEST(Product, Error) {
    // Initial cast fails.
    {
        bool failed = false;
        try {
            sanisizer::product<std::int8_t>(0, 1000);
        } catch (std::overflow_error& e) {
            failed = true;
        }
        EXPECT_TRUE(failed);
    }

    // Product overflows.
    {
        bool failed = false;
        try {
            sanisizer::product<std::uint8_t>(5, 255);
        } catch (std::overflow_error& e) {
            failed = true;
        }
        EXPECT_TRUE(failed);
    }

    // Repeat when the second value is a Constant, which uses a different branch.
    {
        bool failed = false;
        try {
            sanisizer::product<std::uint8_t>(5, sanisizer::Constant<int, 255>());
        } catch (std::overflow_error& e) {
            failed = true;
        }
        EXPECT_TRUE(failed);
    }
}

TEST(Product, Multiple) {
    // Still works with additional arguments.
    EXPECT_EQ(sanisizer::product<std::int16_t>(5, 20, 50), 5000);
    EXPECT_EQ(sanisizer::product<std::int16_t>(sanisizer::Attestation<i32, 50>(5), sanisizer::Attestation<i32, 100>(20), sanisizer::attest_max_by_type<i32>(50)), 5000);
    EXPECT_EQ(sanisizer::product<std::int16_t>(sanisizer::Constant<i32, 5>(), sanisizer::Constant<i32, 20>(), sanisizer::Constant<i32, 50>()), 5000);

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

TEST(ProductUnsafe, Integer) {
    EXPECT_EQ(sanisizer::product_unsafe<std::uint8_t>((i32)5, (i32)20), 100);
    EXPECT_EQ(sanisizer::product_unsafe<std::uint8_t>((u32)5, (i32)20), 100);
    EXPECT_EQ(sanisizer::product_unsafe<std::uint8_t>((i32)5, (u32)20), 100);
    EXPECT_EQ(sanisizer::product_unsafe<std::uint8_t>((u32)5, (u32)20, (u32)2), 200);

    EXPECT_EQ(sanisizer::product_unsafe<std::int64_t>((i32)5, (i32)20), 100);
    EXPECT_EQ(sanisizer::product_unsafe<std::int64_t>((u32)5, (i32)20), 100);
    EXPECT_EQ(sanisizer::product_unsafe<std::int64_t>((i32)5, (u32)20), 100);
    EXPECT_EQ(sanisizer::product_unsafe<std::int64_t>((u32)5, (u32)20, (u32)100), 10000);

    // Works at compile-time.
    static_assert(sanisizer::product_unsafe<std::int64_t>((i32)5, (i32)20) == 100);
}

TEST(ProductUnsafe, Bounded) {
    EXPECT_EQ(sanisizer::product_unsafe<std::uint8_t>(
        sanisizer::set_bound_by_type<std::uint8_t>(5),
        sanisizer::set_bound_by_type<std::uint8_t>(20)
    ), 100);

    EXPECT_EQ(sanisizer::product_unsafe<std::uint8_t>(
        sanisizer::set_bound_by_type<std::uint8_t>(5),
        sanisizer::set_bound_by_type<std::uint8_t>(20), 
        sanisizer::set_bound_by_type<std::uint8_t>(2)
    ), 200);

    // Works at compile-time.
    static_assert(sanisizer::product_unsafe<std::uint8_t>(
        sanisizer::set_bound_by_type<std::uint8_t>(5),
        sanisizer::set_bound_by_type<std::uint8_t>(20)
    ) == 100);
}

TEST(ProductUnsafe, Constant) {
    EXPECT_EQ(sanisizer::product_unsafe<std::uint8_t>(
        sanisizer::Constant<std::uint8_t, 5>(),
        sanisizer::Constant<std::uint8_t, 20>()
    ), 100);

    EXPECT_EQ(sanisizer::product_unsafe<std::uint8_t>(
        sanisizer::Constant<std::uint8_t, 5>(),
        sanisizer::Constant<std::uint8_t, 20>(), 
        sanisizer::Constant<std::uint8_t, 2>()
    ), 200);

    // Works at compile-time.
    static_assert(sanisizer::product_unsafe<std::uint8_t>(
        sanisizer::Constant<std::uint8_t, 5>(),
        sanisizer::Constant<std::uint8_t, 20>()
    ) == 100);
}
