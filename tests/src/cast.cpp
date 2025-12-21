#include <gtest/gtest.h>

#include "sanisizer/cast.hpp"

#include <cstdint>

typedef std::int8_t i8;
typedef std::uint8_t u8;
typedef std::int32_t i32;
typedef std::uint32_t u32;

TEST(Cast, Basic) {
    EXPECT_EQ(sanisizer::cast<std::int32_t>((u32)1), 1);
    EXPECT_EQ(sanisizer::cast<std::uint32_t>((u32)2), 2);
    EXPECT_EQ(sanisizer::cast<std::int32_t>((i32)3), 3);
    EXPECT_EQ(sanisizer::cast<std::uint32_t>((i32)4), 4);

    EXPECT_EQ(sanisizer::cast<std::uint8_t>((u32)10), 10);
    EXPECT_EQ(sanisizer::cast<std::uint32_t>((u8)200), 200);
    EXPECT_EQ(sanisizer::cast<std::int8_t>((u32)100), 100);
    EXPECT_EQ(sanisizer::cast<std::int32_t>((u8)250), 250);

    // Check that the type makes sense.
    static_assert(std::is_same<decltype(sanisizer::cast<std::uint32_t>((i32)1)), std::uint32_t>::value);
    static_assert(std::is_same<decltype(sanisizer::cast<std::int32_t>((u32)1)), std::int32_t>::value);
    static_assert(std::is_same<decltype(sanisizer::cast<std::int8_t>((u32)1)), std::int8_t>::value);
    static_assert(std::is_same<decltype(sanisizer::cast<std::uint8_t>((u32)1)), std::uint8_t>::value);

    // Works with attestations.
    EXPECT_EQ(sanisizer::cast<std::uint8_t>(sanisizer::Attestation<int, true, 100>(10)), 10);
    EXPECT_EQ(sanisizer::cast<std::int64_t>(sanisizer::Attestation<int, true, 100>(100)), 100);

    bool failed = false;
    try {
        sanisizer::cast<std::uint8_t>(256);
    } catch (std::overflow_error& e) {
        failed = true;
    }
    EXPECT_TRUE(failed);

    failed = false;
    try {
        sanisizer::cast<std::uint8_t>(-10);
    } catch (std::out_of_range& e) {
        failed = true;
    }
    EXPECT_TRUE(failed);
}
