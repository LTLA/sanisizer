#include <gtest/gtest.h>

#include "sanisizer/sum.hpp"

#include <cstdint>

// For brevity.
typedef std::int32_t i32;
typedef std::uint32_t u32;

TEST(Sum, Check) {
    static_assert(sanisizer::needs_sum_check<std::int64_t, std::int64_t, std::int64_t>());
    static_assert(sanisizer::needs_sum_check<std::int32_t, std::int64_t, std::int64_t>());
    static_assert(sanisizer::needs_sum_check<std::int64_t, std::int32_t, std::uint64_t>());
    static_assert(sanisizer::needs_sum_check<std::int64_t, std::uint32_t, std::int64_t>());

    static_assert(!sanisizer::needs_sum_check<std::int64_t, std::int32_t, std::int32_t>());
    static_assert(!sanisizer::needs_sum_check<std::int64_t, std::uint32_t, std::uint32_t>());
    static_assert(!sanisizer::needs_sum_check<std::int64_t, sanisizer::Attestation<std::int64_t, 10>, std::int32_t>());
    static_assert(!sanisizer::needs_sum_check<std::int64_t, std::int64_t, sanisizer::Attestation<std::int64_t, 0> >());
}

TEST(Sum, Okay) {
    EXPECT_EQ(sanisizer::sum<std::int64_t>((i32)5, (i32)20), 25);
    EXPECT_EQ(sanisizer::sum<std::int64_t>((u32)5, (i32)20), 25);
    EXPECT_EQ(sanisizer::sum<std::int64_t>((i32)5, (u32)20), 25);
    EXPECT_EQ(sanisizer::sum<std::int64_t>((u32)5, (u32)20), 25);

    EXPECT_EQ(sanisizer::sum<std::uint8_t>((i32)5, (i32)20), 25);
    EXPECT_EQ(sanisizer::sum<std::uint8_t>((u32)5, (i32)20), 25);
    EXPECT_EQ(sanisizer::sum<std::uint8_t>((i32)5, (u32)20), 25);
    EXPECT_EQ(sanisizer::sum<std::uint8_t>((u32)5, (u32)20), 25);

    EXPECT_EQ(sanisizer::sum<std::int8_t>(5, sanisizer::Attestation<i32, 1000>((i32)20)), 25);
    EXPECT_EQ(sanisizer::sum<std::int8_t>(5, sanisizer::Attestation<i32, 100>((i32)20)), 25);
    EXPECT_EQ(sanisizer::sum<std::int8_t>(sanisizer::Attestation<u32, 1000>((u32)20), 5), 25);
    EXPECT_EQ(sanisizer::sum<std::int8_t>(sanisizer::Attestation<u32, 100>((u32)20), 5), 25);

    // Works at compile-time.
    static_assert(sanisizer::sum<std::int64_t>((i32)5, (i32)20) == 25);
}

TEST(Sum, Fail) {
    bool failed = false;
    try {
        sanisizer::sum<std::int8_t>(5, 255);
    } catch (std::overflow_error& e) {
        failed = true;
    }
    EXPECT_TRUE(failed);
}

TEST(Sum, Multiple) {
    // Still works with additional arguments.
    EXPECT_EQ(sanisizer::sum<std::int8_t>(5, 20, 50), 75);
    EXPECT_EQ(sanisizer::sum<std::int8_t>(sanisizer::Attestation<i32, 50>(5), sanisizer::Attestation<i32, 100>(20), sanisizer::attest_max_by_type<i32>(50)), 75);

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

    // Works at compile-time.
    static_assert(sanisizer::sum_unsafe<std::int64_t>((i32)5, (i32)20) == 25);
}
