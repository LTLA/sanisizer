#include <gtest/gtest.h>

#include "sanisizer/cast.hpp"

#include <cstdint>

typedef std::int8_t i8;
typedef std::uint8_t u8;
typedef std::int32_t i32;
typedef std::uint32_t u32;

TEST(PreCast, Integer) {
    // Returns an integer.
    EXPECT_EQ(sanisizer::pre_cast<std::uint32_t>((u32)2), 2);
    EXPECT_EQ(sanisizer::pre_cast<std::int32_t>((i32)3), 3);
    EXPECT_EQ(sanisizer::pre_cast<std::uint32_t>((i32)4), 4);

    EXPECT_EQ(sanisizer::pre_cast<std::uint32_t>((u8)200), 200);
    EXPECT_EQ(sanisizer::pre_cast<std::int32_t>((u8)250), 250);

    // Returns a Bounded.
    EXPECT_EQ(sanisizer::pre_cast<std::int32_t>((u32)1).value, 1);
    EXPECT_EQ(decltype(sanisizer::pre_cast<std::int32_t>((u32)1))::max, 2147483647);
    EXPECT_EQ(sanisizer::pre_cast<std::uint8_t>((u32)10).value, 10);
    EXPECT_EQ(decltype(sanisizer::pre_cast<std::uint8_t>((u32)10))::max, 255);
    EXPECT_EQ(sanisizer::pre_cast<std::int8_t>((u32)100).value, 100);
    EXPECT_EQ(decltype(sanisizer::pre_cast<std::int8_t>((u32)100))::max, 127);

    // Works at compile time.
    static_assert(sanisizer::pre_cast<std::uint8_t>((i8)10) == 10);
    static_assert(sanisizer::pre_cast<std::int8_t>((u8)10).value == 10);
    static_assert(decltype(sanisizer::pre_cast<std::int8_t>((u8)10))::max == 127);

    // Types are preserved.
    static_assert(std::is_same<decltype(sanisizer::pre_cast<std::uint8_t>((i8)10)), i8>::value);
    static_assert(std::is_same<decltype(sanisizer::pre_cast<std::int8_t>((u8)10).value), u8>::value);

    // Checking cast failure for both signed and unsigned destinations.
    {
        bool failed = false;
        try {
            sanisizer::pre_cast<std::uint8_t>(256);
        } catch (std::overflow_error& e) {
            failed = true;
        }
        EXPECT_TRUE(failed);
    }

    {
        bool failed = false;
        try {
            sanisizer::pre_cast<std::int8_t>(128);
        } catch (std::overflow_error& e) {
            failed = true;
        }
        EXPECT_TRUE(failed);
    }
}

TEST(PreCast, Bounded) {
    // No-op as Bound is less than the limit.
    EXPECT_EQ(sanisizer::pre_cast<std::int64_t>(sanisizer::Attestation<std::int32_t, 200>(100)).value, 100);
    EXPECT_EQ(decltype(sanisizer::pre_cast<std::int64_t>(sanisizer::Attestation<std::int32_t, 200>(100)))::max, 200);
    EXPECT_EQ(sanisizer::pre_cast<std::int64_t>(sanisizer::Attestation<std::uint8_t, 200>(100)).value, 100);
    EXPECT_EQ(decltype(sanisizer::pre_cast<std::int64_t>(sanisizer::Attestation<std::uint8_t, 200>(100)))::max, 200);

    // Returns a new Bounded.
    EXPECT_EQ(sanisizer::pre_cast<std::int8_t>(sanisizer::Attestation<std::int32_t, 200>(100)).value, 100);
    EXPECT_EQ(decltype(sanisizer::pre_cast<std::int8_t>(sanisizer::Attestation<std::int32_t, 200>(100)))::max, 127);
    EXPECT_EQ(sanisizer::pre_cast<std::int8_t>(sanisizer::Attestation<std::uint8_t, 200>(100)).value, 100);
    EXPECT_EQ(decltype(sanisizer::pre_cast<std::int8_t>(sanisizer::Attestation<std::uint8_t, 200>(100)))::max, 127);

    // Works at compile time.
    static_assert(sanisizer::pre_cast<std::int64_t>(sanisizer::Attestation<std::int32_t, 200>(100)).value == 100);
    static_assert(decltype(sanisizer::pre_cast<std::int64_t>(sanisizer::Attestation<std::int32_t, 200>(100)))::max == 200);
    static_assert(sanisizer::pre_cast<std::int8_t>(sanisizer::Attestation<std::int32_t, 200>(100)).value == 100);
    static_assert(decltype(sanisizer::pre_cast<std::int8_t>(sanisizer::Attestation<std::int32_t, 200>(100)))::max == 127);

    // Types are preserved.
    static_assert(std::is_same<decltype(sanisizer::pre_cast<std::int64_t>(sanisizer::Attestation<std::int32_t, 200>(100)).value), std::int32_t>::value);
    static_assert(std::is_same<decltype(sanisizer::pre_cast<std::int8_t>(sanisizer::Attestation<std::int16_t, 200>(100)).value), std::int16_t>::value);

    bool failed = false;
    try {
        sanisizer::pre_cast<std::uint8_t>(sanisizer::Attestation<std::int64_t, 1000>(1000));
    } catch (std::overflow_error& e) {
        failed = true;
    }
    EXPECT_TRUE(failed);
}

TEST(PreCast, Constant) {
    // Must be a no-op if it's successful.
    EXPECT_EQ(decltype(sanisizer::pre_cast<std::int64_t>(sanisizer::Constant<std::int32_t, 200>()))::value, 200);
    EXPECT_EQ(decltype(sanisizer::pre_cast<std::int64_t>(sanisizer::Constant<std::uint8_t, 200>()))::value, 200);
    EXPECT_EQ(decltype(sanisizer::pre_cast<std::uint8_t>(sanisizer::Constant<std::int32_t, 200>()))::value, 200);
    EXPECT_EQ(decltype(sanisizer::pre_cast<std::uint8_t>(sanisizer::Constant<std::uint8_t, 200>()))::value, 200);

    // Works at compile time.
    static_assert(decltype(sanisizer::pre_cast<std::int64_t>(sanisizer::Constant<std::int32_t, 200>()))::value == 200);
    static_assert(decltype(sanisizer::pre_cast<std::uint8_t>(sanisizer::Constant<std::int32_t, 200>()))::value == 200);

    // Types are preserved.
    static_assert(std::is_same<decltype(decltype(sanisizer::pre_cast<std::int64_t>(sanisizer::Constant<std::int32_t, 200>()))::value), const std::int32_t>::value);
    static_assert(std::is_same<decltype(decltype(sanisizer::pre_cast<std::uint8_t>(sanisizer::Constant<std::int16_t, 200>()))::value), const std::int16_t>::value);

    bool failed = false;
    try {
        sanisizer::pre_cast<std::uint8_t>(sanisizer::Constant<std::int64_t, 1000>());
    } catch (std::overflow_error& e) {
        failed = true;
    }
    EXPECT_TRUE(failed);
}

TEST(Cast, Basic) {
    // Always returns an integer type.
    EXPECT_EQ(sanisizer::cast<std::int32_t>((u8)200), 200);
    EXPECT_EQ(sanisizer::cast<std::uint8_t>((i32)200), 200);
    EXPECT_EQ(sanisizer::cast<std::int64_t>(sanisizer::Attestation<std::int32_t, 200>(100)), 100);
    EXPECT_EQ(sanisizer::cast<std::uint8_t>(sanisizer::Attestation<std::int32_t, 200>(100)), 100);
}
