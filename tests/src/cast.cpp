#include <gtest/gtest.h>

#include "sanisizer/cast.hpp"

#include <cstdint>

typedef std::int8_t i8;
typedef std::uint8_t u8;
typedef std::int32_t i32;
typedef std::uint32_t u32;
typedef std::int64_t i64;
typedef std::uint64_t u64;

TEST(CanCast, Integer) {
    // Skip check. 
    static_assert(sanisizer::can_cast<u32>((u32)2));
    static_assert(sanisizer::can_cast<i32>((i32)3));
    static_assert(sanisizer::can_cast<u32>((i32)4));
    static_assert(sanisizer::can_cast<u32>((u8)200));
    static_assert(sanisizer::can_cast<i32>((u8)250));

    // No skip check, no overflow.
    static_assert(sanisizer::can_cast<i32>((u32)1));
    static_assert(sanisizer::can_cast<u8>((u32)10));
    static_assert(sanisizer::can_cast<i8>((u32)100));

    // Overflow.
    static_assert(!sanisizer::can_cast<u8>(256));
    static_assert(!sanisizer::can_cast<i8>(128));
}

TEST(CanCast, Bounded) {
    // Skip check. 
    static_assert(sanisizer::can_cast<i64>(sanisizer::Attestation<i32, 200>(100)));
    static_assert(sanisizer::can_cast<i64>(sanisizer::Attestation<u8, 200>(100)));
    static_assert(sanisizer::can_cast<u8>(sanisizer::Attestation<i32, 200>(100)));
    static_assert(sanisizer::can_cast<u8>(sanisizer::Attestation<u8, 200>(100)));

    // No skip check, no overflow.
    static_assert(sanisizer::can_cast<i32>(sanisizer::Attestation<u32, 3000000000>(100)));
    static_assert(sanisizer::can_cast<i8>(sanisizer::Attestation<i32, 200>(100)));

    // Overflow.
    static_assert(!sanisizer::can_cast<u8>(sanisizer::Attestation<i64, 1000>(1000)));
    static_assert(!sanisizer::can_cast<i8>(sanisizer::Attestation<u32, 200>(150)));
}

TEST(CanCast, Constant) {
    // Must be a no-op if it's successful.
    static_assert(sanisizer::can_cast<i64>(sanisizer::Constant<i32, 200>()));
    static_assert(sanisizer::can_cast<i64>(sanisizer::Constant<u8, 200>()));
    static_assert(sanisizer::can_cast<u8>(sanisizer::Constant<i32, 200>()));
    static_assert(sanisizer::can_cast<u8>(sanisizer::Constant<u8, 200>()));

    static_assert(!sanisizer::can_cast<u8>(sanisizer::Constant<i64, 1000>()));
    static_assert(!sanisizer::can_cast<i32>(sanisizer::Constant<u32, 3000000000>()));
}

TEST(Cast, Basic) {
    static_assert(sanisizer::cast<i32>((u8)200) == 200);
    static_assert(sanisizer::cast<u8>((i32)200) == 200);

    // Always returns an integer type.
    static_assert(sanisizer::cast<i64>(sanisizer::Attestation<i32, 200>(100)) == 100);
    static_assert(sanisizer::cast<u8>(sanisizer::Attestation<i32, 200>(100)) == 100);
    static_assert(sanisizer::cast<i64>(sanisizer::Constant<i32, 200>()) == 200);
    static_assert(sanisizer::cast<u8>(sanisizer::Constant<i32, 200>()) == 200);

    bool failed = false;
    try {
        sanisizer::cast<i32>(u32(3000000000));
    } catch (std::overflow_error&) {
        failed = true;
    }
    EXPECT_TRUE(failed);
}
