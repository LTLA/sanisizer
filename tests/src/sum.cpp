#include <gtest/gtest.h>

#include "sanisizer/sum.hpp"

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

TEST(NeedsSumCheck, Integer) {
    static_assert(sanisizer::needs_sum_check<i32, i64, i64>()); // first argument overflows.
    static_assert(sanisizer::needs_sum_check<i64, i64, i32>()); // sum overflows.
    static_assert(!sanisizer::needs_sum_check<u8, i8, i8>());   // no overflow.
}

TEST(NeedsSumCheck, Bounded) {
    static_assert(sanisizer::needs_sum_check<u8, Bound<i64, 256>, Bound<i8, 123> >()); // first argument overflows.
    static_assert(sanisizer::needs_sum_check<u8, Bound<i64, 255>, Bound<i8, 1> >());   // sum overflows.
    static_assert(!sanisizer::needs_sum_check<u8, Bound<i64, 235>, Bound<i8, 20> >()); // no overflow.
}

TEST(NeedsSumCheck, Constant) {
    static_assert(sanisizer::needs_sum_check<i8, Const<u8, 128>, Const<i8, 123> >()); // first argument overflows.
    static_assert(sanisizer::needs_sum_check<i8, Const<i8, 127>, Const<i8, 1> >());   // sum overflows.
    static_assert(!sanisizer::needs_sum_check<i8, Const<i8, 107>, Const<i8, 20> >()); // no overflow.
}

/********************************************/

TEST(DoesSumOverflowOrdered, Integer) {
    static_assert(sanisizer::does_sum_overflow_ordered<i32>(i64(2147483648), i64(20))); // first argument overflows.
    static_assert(sanisizer::does_sum_overflow_ordered<i32>(i32(2147483640), i32(8)));  // sum overflows.
    static_assert(!sanisizer::does_sum_overflow_ordered<u8>(i8(127), i8(127)));         // no overflow.
}

TEST(DoesSumOverflowOrdered, Bounded) {
    static_assert(sanisizer::does_sum_overflow_ordered<u8>(Bound<i64, 300>(256), Bound<i8, 100>(1)));   // first argument overflows.
    static_assert(sanisizer::does_sum_overflow_ordered<u8>(Bound<i64, 300>(255), Bound<i8, 100>(1)));   // sum overflows.
    static_assert(!sanisizer::does_sum_overflow_ordered<u8>(Bound<i64, 235>(199), Bound<i8, 100>(56))); // no overflow.
}

TEST(DoesSumOverflowOrdered, Constant) {
    //static_assert(sanisizer::does_sum_overflow_ordered<i8>(Const<u8, 128>(), Const<i8, 123>())); // first argument overflows. // skipped due to clang warning. 

    static_assert(sanisizer::does_sum_overflow_ordered<i8>(Const<i8, 127>(), Const<i8, 1>()));   // sum overflows.
    static_assert(!sanisizer::does_sum_overflow_ordered<i8>(Const<i8, 107>(), Const<i8, 20>())); // no overflow.
}

/********************************************/

TEST(DoesSumOverflow, FirstConstant) {
    static_assert(sanisizer::does_sum_overflow<i32>(Const<u32, 2147483628>(), i64(20))); 
    static_assert(sanisizer::does_sum_overflow<i32>(Const<u32, 2147483628>(), Bound<i64, 200>(20))); // fails against Bounded.
    static_assert(sanisizer::does_sum_overflow<i32>(Const<u32, 2147483628>(), Const<i64, 200>())); // fails against Constant.

    static_assert(!sanisizer::does_sum_overflow<i32>(Const<u32, 2147483627>(), i64(20))); 
    static_assert(!sanisizer::does_sum_overflow<i32>(Const<u32, 2147483627>(), Bound<i64, 200>(20))); // works against Bounded.
    static_assert(!sanisizer::does_sum_overflow<i32>(Const<u32, 2147483627>(), Const<i64, 20>())); // works against Constant.
}

TEST(DoesSumOverflow, SecondConstant) {
    static_assert(sanisizer::does_sum_overflow<i32>(i64(20), Const<u32, 2147483628>())); 
    static_assert(sanisizer::does_sum_overflow<i32>(Bound<i64, 200>(20), Const<u32, 2147483628>())); // fails against Bounded.

    static_assert(!sanisizer::does_sum_overflow<i32>(i64(20), Const<u32, 2147483627>())); 
    static_assert(!sanisizer::does_sum_overflow<i32>(Bound<u8, 200>(20), Const<u32, 2147483627>())); // works against Bounded.

    // No need to check first Constant, as that was handled in the previous test.
}

TEST(DoesSumOverflow, FirstMaxSmaller) {
    static_assert(sanisizer::does_sum_overflow<i32>(i32(2147483647), u32(1))); 
    static_assert(sanisizer::does_sum_overflow<i32>(Bound<i64, 100>(1), Bound<u32, 2147483648>(2147483647))); // works for Bounded.

    static_assert(!sanisizer::does_sum_overflow<i32>(i32(2147483646), u32(1))); 
    static_assert(!sanisizer::does_sum_overflow<i32>(Bound<u64, 100>(1), Bound<i64, 2147483648>(2147483646))); // works for Bounded.
}

TEST(DoesSumOverflow, SecondMaxSmaller) {
    static_assert(sanisizer::does_sum_overflow<i32>(u32(100), i32(2147483548)));
    static_assert(sanisizer::does_sum_overflow<i32>(Bound<u32, 3000000000>(2147483548), Bound<u64, 100>(100))); // works for Bounded.

    static_assert(!sanisizer::does_sum_overflow<i32>(u32(100), i32(2147483547))); 
    static_assert(!sanisizer::does_sum_overflow<i32>(Bound<u32, 3000000000>(2147483547), Bound<i64, 100>(100))); // works for Bounded.
}

TEST(DoesSumOverflow, NeitherMaxSmaller) {
    static_assert(sanisizer::does_sum_overflow<i8>(u8(100), u8(200)));
    static_assert(sanisizer::does_sum_overflow<i8>(Bound<u8, 128>(100), Bound<u8, 230>(200))); // works for Bounded.

    static_assert(!sanisizer::does_sum_overflow<i8>(u8(100), u8(27)));
    static_assert(!sanisizer::does_sum_overflow<i8>(Bound<u8, 128>(120), Bound<u8, 230>(7))); // works for Bounded.
}

/********************************************/

template<typename Integer_, typename Expected_>
constexpr bool is_sum_expected(std::optional<Integer_> x, Expected_ expected) {
    return x.has_value() && static_cast<typename std::make_unsigned<Integer_>::type>(*x) == static_cast<typename std::make_unsigned<Expected_>::type>(expected);
}

TEST(SumProtected, TwoInteger) {
    static_assert(!sanisizer::sum_protected<u8>(u32(100), i64(200)).has_value());

    static_assert(is_sum_expected(sanisizer::sum_protected<i32>(u32(50), i64(20)), 70)); // checks not elided, no overflow.

    constexpr auto payload = sanisizer::sum_protected<u32>(i32(50), i8(20)); // checks elided, Bounded emitted.
    static_assert(payload.has_value());
    static_assert(payload->value == 70);
    static_assert(sanisizer::I<decltype(*payload)>::max == 2147483774u);
}

TEST(SumProtected, ThreeInteger) {
    static_assert(!sanisizer::sum_protected<i8>(u32(100), i64(20), i32(8)).has_value());
    static_assert(!sanisizer::sum_protected<i8>(u32(100), i64(30), i32(8)).has_value()); // overflow in first operation causes a short-circuit.

    static_assert(is_sum_expected(sanisizer::sum_protected<u8>(i8(100), i8(20), i32(7)), 127)); // checks elided for first but not second operation.
    static_assert(is_sum_expected(sanisizer::sum_protected<u8>(u32(100), i64(20), i32(7)), 127)); // checks elided for no operations.

    constexpr auto payload = sanisizer::sum_protected<u32>(i32(50), i8(20), i8(100)); // checks elided for all operations.
    static_assert(payload.has_value());
    static_assert(payload->value == 170);
    static_assert(sanisizer::I<decltype(*payload)>::max == 2147483901u);
}

TEST(SumProtected, TwoBounded) {
    static_assert(!sanisizer::sum_protected<u8>(Bound<u32, 200>(100), Bound<i64, 200>(200)).has_value());

    static_assert(is_sum_expected(sanisizer::sum_protected<i8>(Bound<i32, 100>(50), Bound<i64, 30>(20)), 70)); // checks not elided, no overflow.

    constexpr auto payload = sanisizer::sum_protected<i8>(Bound<i32, 71>(50), Bound<i8, 55>(20)); // checks elided, Bounded emitted.
    static_assert(payload.has_value());
    static_assert(payload->value == 70);
    static_assert(sanisizer::I<decltype(*payload)>::max == 126);
}

TEST(SumProtected, ThreeBounded) {
    static_assert(!sanisizer::sum_protected<u8>(Bound<i32, 300>(230), Bound<i64, 200>(20), Bound<i32, 10>(6)).has_value());
    static_assert(!sanisizer::sum_protected<u8>(Bound<u32, 200>(156), Bound<i64, 100>(100), Bound<i32, 10>(1)).has_value()); // overflow in first operation causes a short-circuit.

    static_assert(is_sum_expected(sanisizer::sum_protected<i8>(Bound<i8, 100>(100), Bound<i32, 25>(20), Bound<i64, 200>(7)), 127)); // checks elided for first but not second operation.
    static_assert(is_sum_expected(sanisizer::sum_protected<i8>(Bound<u32, 110>(100), Bound<i64, 20>(20), Bound<i32, 50>(7)), 127)); // checks elided for no operations.

    constexpr auto payload = sanisizer::sum_protected<i8>(Bound<i32, 70>(50), Bound<i8, 20>(20), Bound<i8, 6>(5)); // checks elided for all operations.
    static_assert(payload.has_value());
    static_assert(payload->value == 75);
    static_assert(sanisizer::I<decltype(*payload)>::max == 96);
}

TEST(SumProtected, TwoConstant) {
    static_assert(!sanisizer::sum_protected<u8>(Const<u32, 200>(), Const<i64, 60>()).has_value());

    constexpr auto payload = sanisizer::sum_protected<i8>(Const<i32, 70>(), Const<i8, 57>()); // checks elided, Constant emitted.
    static_assert(payload.has_value());
    static_assert(sanisizer::I<decltype(*payload)>::value == 127);
}

TEST(SumProtected, ThreeConstant) {
    static_assert(!sanisizer::sum_protected<u8>(Const<u32, 200>(), Const<i64, 50>(), Const<u32, 10>()).has_value());
    static_assert(!sanisizer::sum_protected<u8>(Const<u32, 200>(), Const<i64, 56>(), Const<u32, 1>()).has_value()); // overflow in first operation causes a short-circuit. 

    constexpr auto payload = sanisizer::sum_protected<i8>(Const<i32, 30>(), Const<i8, 57>(), Const<u8, 40>()); // checks elided, Constant emitted.
    static_assert(payload.has_value());
    static_assert(sanisizer::I<decltype(*payload)>::value == 127);
}

/********************************************/

TEST(SumMaybe, Basic) {
    static_assert(!sanisizer::sum_maybe<i8>(u32(100), i32(100)).has_value());
    static_assert(!sanisizer::sum_maybe<i8>(u32(100), i32(10), i8(20)).has_value()); 

    static_assert(is_sum_expected(sanisizer::sum_maybe<i8>(u32(27), i32(100)), 127));
    static_assert(is_sum_expected(sanisizer::sum_maybe<u8>(u32(27), i32(100), u8(128)), 255));

    // Check that an integer is actually returned, instead of being wrapped in a Bounded.
    static_assert(is_sum_expected(sanisizer::sum_maybe<u8>(i8(27), i8(50)), 77));
    static_assert(is_sum_expected(sanisizer::sum_maybe<u32>(i8(27), i8(50), i32(100)), 177));

    // Check that an integer is actually returned, instead of being wrapped in a Constant.
    static_assert(is_sum_expected(sanisizer::sum_maybe<u8>(Const<i8, 27>(), Const<i8, 50>()), 77));
    static_assert(is_sum_expected(sanisizer::sum_maybe<i32>(Const<i8, 27>(), Const<i8, 50>(), Const<i8, 100>()), 177));
}

TEST(Sum, Basic) {
    {
        bool failed = false;
        try {
            sanisizer::sum<i8>(u32(100), i32(100));
        } catch (std::overflow_error& e) {
            failed = true;
        }
        EXPECT_TRUE(failed);
    }

    {
        bool failed = false;
        try {
            sanisizer::sum<i8>(u32(100), i32(10), i8(20));
        } catch (std::overflow_error& e) {
            failed = true;
        }
        EXPECT_TRUE(failed);
    }

    static_assert(sanisizer::sum<i8>(u32(27), i32(100)) == 127);
    static_assert(sanisizer::sum<u8>(u32(27), i32(100), u8(128)) == 255u);

    // Check that an integer is actually returned, instead of being wrapped in a Bounded.
    static_assert(sanisizer::sum<u8>(i8(27), i8(50)) == 77u);
    static_assert(sanisizer::sum<u32>(i8(27), i8(50), i32(100)) == 177u);

    // Check that an integer is actually returned, instead of being wrapped in a Const.
    static_assert(sanisizer::sum<i8>(Const<i8, 27>(), Const<i8, 50>()) == 77);
    static_assert(sanisizer::sum<i32>(Const<i8, 27>(), Const<i8, 50>(), Const<i8, 100>()) == 177);
}

/********************************************/

TEST(SumUnsafe, Integer) {
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

TEST(SumUnsafe, Bounded) {
    EXPECT_EQ(sanisizer::sum_unsafe<std::uint8_t>(
        sanisizer::set_bound_by_type<std::uint8_t>(5),
        sanisizer::set_bound_by_type<std::uint8_t>(20)
    ), 25);

    EXPECT_EQ(sanisizer::sum_unsafe<std::uint8_t>(
        sanisizer::set_bound_by_type<std::uint8_t>(5),
        sanisizer::set_bound_by_type<std::uint8_t>(20), 
        sanisizer::set_bound_by_type<std::uint8_t>(2)
    ), 27);

    static_assert(sanisizer::sum_unsafe<std::uint8_t>(
        sanisizer::set_bound_by_type<std::uint8_t>(5),
        sanisizer::set_bound_by_type<std::uint8_t>(20)
    ) == 25);

}

TEST(SumUnsafe, Constant) {
    EXPECT_EQ(sanisizer::sum_unsafe<std::uint8_t>(
        Const<std::uint8_t, 5>(),
        Const<std::uint8_t, 20>()
    ), 25);

    EXPECT_EQ(sanisizer::sum_unsafe<std::uint8_t>(
        Const<std::uint8_t, 5>(),
        Const<std::uint8_t, 20>(), 
        Const<std::uint8_t, 2>()
    ), 27);

    static_assert(sanisizer::sum_unsafe<std::uint8_t>(
        Const<std::uint8_t, 5>(),
        Const<std::uint8_t, 20>()
    ) == 25);
}

/********************************************/

TEST(PlusOne, Basic) {
    EXPECT_EQ(sanisizer::plus_one<int>(100), 101);
    EXPECT_EQ(sanisizer::plus_one<std::uint8_t>((std::int8_t)127), 128);
    EXPECT_EQ(sanisizer::plus_one<int>(sanisizer::set_bound<std::int8_t, 100>(50)), 51);
    EXPECT_EQ(sanisizer::plus_one<int>(sanisizer::set_bound<std::int8_t, 100>(100)), 101);
    EXPECT_EQ(sanisizer::plus_one<int>(Const<std::int8_t, 100>()), 101);

    bool failed = false;
    try {
        sanisizer::plus_one<std::uint16_t>(65535);
    } catch (std::overflow_error&) {
        failed = true;
    }
    EXPECT_TRUE(failed);
}

TEST(Increment, Basic) {
    {
        std::uint8_t a = 254;
        EXPECT_EQ(sanisizer::increment(a), 255);
        EXPECT_EQ(a, 255);
    }

    bool failed = false;
    try {
        std::uint8_t a = 255;
        sanisizer::increment(a);
    } catch (std::overflow_error&) {
        failed = true;
    }
    EXPECT_TRUE(failed);
}
