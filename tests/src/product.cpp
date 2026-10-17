#include <gtest/gtest.h>

#include "sanisizer/product.hpp"

#include <cstdint>

// For brevity.
typedef std::int8_t i8;
typedef std::uint8_t u8;
typedef std::int32_t i32;
typedef std::uint32_t u32;
typedef std::int64_t i64;
typedef std::uint64_t u64;

template<typename Integer_, Integer_ max_>
using Bound = sanisizer::Bounded<Integer_, max_>; 

template<typename Integer_, Integer_ max_>
using Const = sanisizer::Constant<Integer_, max_>;

/********************************************/

TEST(NeedsProductCheck, Integer) {
    static_assert(sanisizer::needs_product_check<i32, i64, i64>());  // first argument overflows.
    static_assert(sanisizer::needs_product_check<i64, u32, u32>());  // product overflows.
    static_assert(!sanisizer::needs_product_check<i64, i32, u32>()); // no overflow.
}

TEST(NeedsProductCheck, Bounded) {
    static_assert(sanisizer::needs_product_check<u8, Bound<i64, 256>, Bound<i8, 123> >()); // first argument overflows.
    static_assert(sanisizer::needs_product_check<u8, Bound<i64, 0>, Bound<i32, 300> >());  // first argument zero, with overflow.
    static_assert(!sanisizer::needs_product_check<u8, Bound<i64, 0>, Bound<u8, 255> >());  // first argument zero, no overflow.
    static_assert(sanisizer::needs_product_check<u8, Bound<i64, 16>, Bound<i8, 16> >());   // product overflows.
    static_assert(!sanisizer::needs_product_check<u8, Bound<i64, 17>, Bound<i8, 15> >());  // no overflow.
}

TEST(NeedsProductCheck, Constant) {
    static_assert(sanisizer::needs_product_check<i8, Const<u8, 128>, Const<i8, 123> >()); // first argument overflows.
    static_assert(sanisizer::needs_product_check<i8, Const<i8, 0>, Const<i32, 128> >());  // first argument zero, with overflow.
    static_assert(!sanisizer::needs_product_check<i8, Const<i8, 0>, Const<i8, 127> >());  // first argument zero, no overflows.
    static_assert(sanisizer::needs_product_check<i8, Const<i8, 8>, Const<i8, 16> >());    // product overflows.
    static_assert(!sanisizer::needs_product_check<u8, Const<i8, 5>, Const<i8, 51> >());   // no overflow.
}

/********************************************/

TEST(DoesProductOverflowOrdered, Integer) {
    static_assert(sanisizer::does_product_overflow_ordered<i32>(i64(2147483648), i64(1)));           // first argument overflows.
    static_assert(sanisizer::does_product_overflow_ordered<i32>(i64(0), i64(2147483648)));           // first argument zero, with overflow.
    static_assert(!sanisizer::does_product_overflow_ordered<i32>(i64(0), i64(2147483647)));          // first argument zero, no overflow.
    static_assert(sanisizer::does_product_overflow_ordered<i64>(u32(4294967295), u32(4294967295)));  // product overflows.
    static_assert(!sanisizer::does_product_overflow_ordered<i64>(i32(2147483647), u32(4294967295))); // no overflow.
}

TEST(DoesProductOverflowOrdered, Bounded) {
    static_assert(sanisizer::does_product_overflow_ordered<u8>(Bound<i64, 300>(256), Bound<i8, 100>(10)));  // first argument overflows.
    static_assert(sanisizer::does_product_overflow_ordered<u8>(Bound<i64, 300>(0), Bound<i32, 1000>(300))); // first argument zero, with overflow.
    static_assert(!sanisizer::does_product_overflow_ordered<u8>(Bound<i64, 300>(0), Bound<u32, 300>(200))); // first argument zero, no overflow.
    static_assert(sanisizer::does_product_overflow_ordered<u8>(Bound<i64, 50>(32), Bound<i8, 20>(8)));      // product overflows.
    static_assert(!sanisizer::does_product_overflow_ordered<u8>(Bound<i8, 30>(20), Bound<i8, 20>(12)));     // no overflow.
}

TEST(DoesProductOverflowOrdered, Constant) {
    //static_assert(sanisizer::does_product_overflow_ordered<i8>(Const<u8, 128>(), Const<u8, 1>()));  // first argument overflows. // skipped due to clang warning.
    static_assert(sanisizer::does_product_overflow_ordered<i8>(Const<i8, 0>(), Const<i32, 128>())); // first argument zero, with overflow.
    static_assert(!sanisizer::does_product_overflow_ordered<i8>(Const<i8, 0>(), Const<i8, 127>())); // first argument zero, no overflows.
    static_assert(sanisizer::does_product_overflow_ordered<i8>(Const<i8, 4>(), Const<i8, 32>()));   // product overflows.
    static_assert(!sanisizer::does_product_overflow_ordered<u8>(Const<i8, 3>(), Const<i8, 85>()));  // no overflow.
}

/********************************************/

TEST(DoesProductOverflow, FirstConstant) {
    static_assert(sanisizer::does_product_overflow<i32>(Const<u32, 1073741824>(), i64(2))); 
    static_assert(sanisizer::does_product_overflow<i32>(Const<u32, 1073741824>(), Bound<i64, 20>(2))); // fails against Bounded.
    static_assert(sanisizer::does_product_overflow<i32>(Const<u32, 1073741824>(), Const<i64, 2>()));  // fails against Constant.

    static_assert(!sanisizer::does_product_overflow<u8>(Const<u32, 15>(), i64(17))); 
    static_assert(!sanisizer::does_product_overflow<u8>(Const<u32, 15>(), Bound<i64, 20>(17))); // works against Bounded.
    static_assert(!sanisizer::does_product_overflow<u8>(Const<u32, 17>(), Const<i64, 15>())); // works against Constant.
}

TEST(DoesProductOverflow, SecondConstant) {
    static_assert(sanisizer::does_product_overflow<i32>(i64(65536), Const<u32, 32768>())); 
    static_assert(sanisizer::does_product_overflow<i32>(Bound<i64, 100000>(65536), Const<u32, 32768>())); // fails against Bounded.

    static_assert(!sanisizer::does_product_overflow<i32>(i64(3), Const<u32, 85>())); 
    static_assert(!sanisizer::does_product_overflow<i32>(Bound<u8, 200>(85), Const<u32, 3>())); // works against Bounded.

    // No need to check first Constant, as that was handled in the previous test.
}

TEST(DoesProductOverflow, FirstMaxSmaller) {
    static_assert(sanisizer::does_product_overflow<i32>(i32(65536), u32(32768))); 
    static_assert(sanisizer::does_product_overflow<i32>(Bound<i64, 40000>(32768), Bound<u32, 100000>(65536))); // works for Bounded.

    static_assert(!sanisizer::does_product_overflow<u32>(i32(65535), u32(65537))); 
    static_assert(!sanisizer::does_product_overflow<u32>(Bound<u64, 100000>(65535), Bound<i64, 200000>(65537))); // works for Bounded.
}

TEST(DoesProductOverflow, SecondMaxSmaller) {
    static_assert(sanisizer::does_product_overflow<i32>(u32(65536), i32(32768)));
    static_assert(sanisizer::does_product_overflow<i32>(Bound<u32, 100000>(65536), Bound<u64, 50000>(32768))); // works for Bounded.

    static_assert(!sanisizer::does_product_overflow<u8>(u32(3), i32(85))); 
    static_assert(!sanisizer::does_product_overflow<u8>(Bound<u32, 200>(3), Bound<i64, 100>(85))); // works for Bounded.
}

TEST(DoesProductOverflow, NeitherMaxSmaller) {
    static_assert(sanisizer::does_product_overflow<i8>(u8(10), u8(20)));
    static_assert(sanisizer::does_product_overflow<i8>(Bound<u8, 128>(10), Bound<u8, 230>(20))); // works for Bounded.

    static_assert(!sanisizer::does_product_overflow<i8>(u8(10), u8(12)));
    static_assert(!sanisizer::does_product_overflow<i8>(Bound<u8, 150>(10), Bound<u8, 200>(12))); // works for Bounded.
}

/********************************************/

template<typename Integer_, typename Expected_>
constexpr bool is_product_expected(std::optional<Integer_> x, Expected_ expected) {
    return x.has_value() && static_cast<typename std::make_unsigned<Integer_>::type>(*x) == static_cast<typename std::make_unsigned<Expected_>::type>(expected);
}

TEST(ProductProtected, TwoInteger) {
    static_assert(!sanisizer::product_protected<u8>(u32(13), i64(20)).has_value());

    static_assert(is_product_expected(sanisizer::product_protected<i32>(u32(20), i64(10)), 200)); // checks not elided, no overflow.

    constexpr auto payload = sanisizer::product_protected<u32>(u8(50), i8(20)); // checks elided, Bounded emitted.
    static_assert(payload.has_value());
    static_assert(payload->value == 1000u);
    static_assert(sanisizer::I<decltype(*payload)>::max == 32385u);
}

TEST(ProductProtected, ThreeInteger) {
    static_assert(!sanisizer::product_protected<i8>(u32(5), i64(6), i32(8)).has_value());
    static_assert(!sanisizer::product_protected<i8>(u32(5), i64(30), i32(8)).has_value()); // overflow in first operation causes a short-circuit.

    static_assert(is_product_expected(sanisizer::product_protected<u8>(i8(5), i8(17), i32(3)), 255)); // checks elided for first but not second operation.
    static_assert(is_product_expected(sanisizer::product_protected<u8>(u32(17), i64(5), i32(3)), 255)); // checks elided for no operations.

    constexpr auto payload = sanisizer::product_protected<i32>(i8(10), u8(20), i8(100)); // checks elided for all operations.
    static_assert(payload.has_value());
    static_assert(payload->value == 20000);
    static_assert(sanisizer::I<decltype(*payload)>::max == 4112895);
}

TEST(ProductProtected, TwoBounded) {
    static_assert(!sanisizer::product_protected<u8>(Bound<u32, 200>(10), Bound<i64, 200>(30)).has_value());

    static_assert(is_product_expected(sanisizer::product_protected<i8>(Bound<i32, 10>(5), Bound<i64, 30>(25)), 125)); // checks not elided, no overflow.

    constexpr auto payload = sanisizer::product_protected<i8>(Bound<i32, 9>(3), Bound<i8, 10>(7)); // checks elided, Bounded emitted.
    static_assert(payload.has_value());
    static_assert(payload->value == 21);
    static_assert(sanisizer::I<decltype(*payload)>::max == 90);
}

TEST(ProductProtected, ThreeBounded) {
    static_assert(!sanisizer::product_protected<u8>(Bound<i32, 300>(3), Bound<i64, 200>(20), Bound<i32, 10>(5)).has_value());
    static_assert(!sanisizer::product_protected<u8>(Bound<u32, 200>(20), Bound<i64, 100>(15), Bound<i32, 10>(1)).has_value()); // overflow in first operation causes a short-circuit.

    static_assert(is_product_expected(sanisizer::product_protected<i8>(Bound<i8, 20>(10), Bound<i32, 5>(2), Bound<i64, 200>(6)), 120)); // checks elided for first but not second operation.
    static_assert(is_product_expected(sanisizer::product_protected<i8>(Bound<u32, 30>(10), Bound<i64, 40>(4), Bound<i32, 50>(3)), 120)); // checks elided for no operations.

    constexpr auto payload = sanisizer::product_protected<i8>(Bound<i32, 5>(3), Bound<i8, 8>(4), Bound<i8, 3>(2)); // checks elided for all operations.
    static_assert(payload.has_value());
    static_assert(payload->value == 24);
    static_assert(sanisizer::I<decltype(*payload)>::max == 120);
}

TEST(ProductProtected, TwoConstant) {
    static_assert(!sanisizer::product_protected<u8>(Const<u32, 4>(), Const<i64, 64>()).has_value());

    constexpr auto payload = sanisizer::product_protected<i8>(Const<i32, 5>(), Const<i8, 25>()); // checks elided, Constant emitted.
    static_assert(payload.has_value());
    static_assert(sanisizer::I<decltype(*payload)>::value == 125);
}

TEST(ProductProtected, ThreeConstant) {
    static_assert(!sanisizer::product_protected<u8>(Const<u32, 4>(), Const<i64, 2>(), Const<u32, 32>()).has_value());
    static_assert(!sanisizer::product_protected<u8>(Const<u32, 32>(), Const<i64, 8>(), Const<u32, 1>()).has_value()); // overflow in first operation causes a short-circuit. 

    constexpr auto payload = sanisizer::product_protected<i8>(Const<i32, 25>(), Const<i8, 1>(), Const<u8, 5>()); // checks elided, Constant emitted.
    static_assert(payload.has_value());
    static_assert(sanisizer::I<decltype(*payload)>::value == 125);
}

/********************************************/

TEST(ProductMaybe, Basic) {
    static_assert(!sanisizer::product_maybe<i8>(u32(13), i32(10)).has_value());
    static_assert(!sanisizer::product_maybe<i8>(u32(5), i32(15), i8(2)).has_value()); 

    static_assert(is_product_expected(sanisizer::product_maybe<i8>(u32(25), i32(5)), 125));
    static_assert(is_product_expected(sanisizer::product_maybe<u8>(u32(5), i32(17), u8(3)), 255));

    // Check that an integer is actually returned, instead of being wrapped in a Bounded.
    static_assert(is_product_expected(sanisizer::product_maybe<u8>(i8(15), i8(10)), 150));
    static_assert(is_product_expected(sanisizer::product_maybe<u32>(i8(10), i8(20), i32(30)), 6000));

    // Check that an integer is actually returned, instead of being wrapped in a Constant.
    static_assert(is_product_expected(sanisizer::product_maybe<u8>(Const<i8, 15>(), Const<i8, 17>()), 255));
    static_assert(is_product_expected(sanisizer::product_maybe<i32>(Const<i8, 17>(), Const<i8, 5>(), Const<i8, 3>()), 255));
}

TEST(Product, Basic) {
    {
        bool failed = false;
        try {
            sanisizer::product<i8>(u32(100), i32(100));
        } catch (std::overflow_error& e) {
            failed = true;
        }
        EXPECT_TRUE(failed);
    }

    {
        bool failed = false;
        try {
            sanisizer::product<i8>(u32(100), i32(10), i8(20));
        } catch (std::overflow_error& e) {
            failed = true;
        }
        EXPECT_TRUE(failed);
    }

    static_assert(sanisizer::product<i8>(u32(27), i32(3)) == 81);
    static_assert(sanisizer::product<u8>(u32(8), i32(5), u8(3)) == 120u);

    // Check that an integer is actually returned, instead of being wrapped in a Bounded.
    static_assert(sanisizer::product<u8>(i8(25), i8(10)) == 250u);
    static_assert(sanisizer::product<u32>(i8(3), i8(5), i32(8)) == 120u);

    // Check that an integer is actually returned, instead of being wrapped in a Const.
    static_assert(sanisizer::product<i8>(Const<i8, 8>(), Const<i8, 15>()) == 120);
    static_assert(sanisizer::product<i32>(Const<i8, 4>(), Const<i8, 15>(), Const<i8, 2>()) == 120);
}

/********************************************/

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
