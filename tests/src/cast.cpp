#include <gtest/gtest.h>

#include "sanisizer/cast.hpp"

#include <cstdint>

typedef std::int8_t i8;
typedef std::uint8_t u8;
typedef std::int32_t i32;
typedef std::uint32_t u32;

TEST(Cast, Integer) {
    EXPECT_EQ(sanisizer::cast<std::int32_t>((u32)1), 1);
    EXPECT_EQ(sanisizer::cast<std::uint32_t>((u32)2), 2);
    EXPECT_EQ(sanisizer::cast<std::int32_t>((i32)3), 3);
    EXPECT_EQ(sanisizer::cast<std::uint32_t>((i32)4), 4);

    EXPECT_EQ(sanisizer::cast<std::uint8_t>((u32)10), 10);
    EXPECT_EQ(sanisizer::cast<std::uint32_t>((u8)200), 200);
    EXPECT_EQ(sanisizer::cast<std::int8_t>((u32)100), 100);
    EXPECT_EQ(sanisizer::cast<std::int32_t>((u8)250), 250);

    // Works at compile time.
    static_assert(sanisizer::cast<std::uint8_t>(10) == 10);

    // Check that the type makes sense.
    static_assert(std::is_same<decltype(sanisizer::cast<std::uint32_t>((i32)1)), std::uint32_t>::value);
    static_assert(std::is_same<decltype(sanisizer::cast<std::int32_t>((u32)1)), std::int32_t>::value);
    static_assert(std::is_same<decltype(sanisizer::cast<std::int8_t>((u32)1)), std::int8_t>::value);
    static_assert(std::is_same<decltype(sanisizer::cast<std::uint8_t>((u32)1)), std::uint8_t>::value);

    bool failed = false;
    try {
        sanisizer::cast<std::uint8_t>(256);
    } catch (std::overflow_error& e) {
        failed = true;
    }
    EXPECT_TRUE(failed);
}

TEST(Cast, Bounded) {
    EXPECT_EQ(sanisizer::cast<std::uint8_t>(sanisizer::Attestation<int, 100>(10)), 10);
    EXPECT_EQ(sanisizer::cast<std::int64_t>(sanisizer::Attestation<int, 100>(100)), 100);

    // Works at compile time.
    static_assert(sanisizer::cast<std::uint8_t>(sanisizer::Attestation<int, 100>(10)) == 10);

    // Check that the type makes sense.
    static_assert(std::is_same<decltype(sanisizer::cast<std::uint8_t>(sanisizer::Attestation<int, 100>(10))), std::uint8_t>::value);
    static_assert(std::is_same<decltype(sanisizer::cast<std::int32_t>(sanisizer::Attestation<int, 1000>(1000))), std::int32_t>::value);

    bool failed = false;
    try {
        sanisizer::cast<std::uint8_t>(sanisizer::Attestation<std::int64_t, 1000>(1000));
    } catch (std::overflow_error& e) {
        failed = true;
    }
    EXPECT_TRUE(failed);
}

TEST(Cast, Constant) {
    EXPECT_EQ(sanisizer::cast<std::uint8_t>(sanisizer::Constant<int, 10>()), 10);
    EXPECT_EQ(sanisizer::cast<std::int64_t>(sanisizer::Constant<int, 100>()), 100);

    // Works at compile time.
    static_assert(sanisizer::cast<std::uint8_t>(sanisizer::Constant<int, 100>()) == 100);

    // Check that the type makes sense.
    static_assert(std::is_same<decltype(sanisizer::cast<std::uint8_t>(sanisizer::Constant<int, 100>())), std::uint8_t>::value);
    static_assert(std::is_same<decltype(sanisizer::cast<std::int32_t>(sanisizer::Constant<int, 1000>())), std::int32_t>::value);

    bool failed = false;
    try {
        sanisizer::cast<std::uint8_t>(sanisizer::Constant<std::int64_t, 1000>());
    } catch (std::overflow_error& e) {
        failed = true;
    }
    EXPECT_TRUE(failed);
}
