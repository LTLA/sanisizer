#include <gtest/gtest.h>

#include <type_traits>
#include <cstdint>

#include "sanisizer/Bounded.hpp"
#include "sanisizer/generics.hpp"
#include "sanisizer/comparisons.hpp"

TEST(Bounded, CompileTime) {
    {
        constexpr sanisizer::Bounded<int, 20> val(5);
        static_assert(std::is_same<int, typename decltype(val)::Integer>::value);
        static_assert(val.value == 5);
        static_assert(decltype(val)::max == 20);
    }

    {
        constexpr sanisizer::Bounded<unsigned, 100> val(10u);
        static_assert(std::is_same<unsigned, typename decltype(val)::Integer>::value);
        static_assert(val.value == 10);
        static_assert(decltype(val)::max == 100);
    }

    {
        constexpr sanisizer::Bounded<long, 1000> val(20l);
        static_assert(std::is_same<long, typename decltype(val)::Integer>::value);
        static_assert(val.value == 20);
        static_assert(decltype(val)::max == 1000);
    }

    static_assert(sanisizer::is_Bounded<sanisizer::Bounded<int, 10> >::value);
    static_assert(sanisizer::is_Bounded<const sanisizer::Bounded<int, 10> >::value);
    static_assert(!sanisizer::is_Bounded<int>::value);
}

TEST(Bounded, RunTime) {
    sanisizer::Bounded<int, 20> val(5);
    EXPECT_EQ(val.value, 5);
}

/**************************************/

typedef std::int8_t i8;
typedef std::uint8_t u8;
typedef std::int32_t i32;
typedef std::uint32_t u32;

TEST(SetBoundByType, IntegerNoOp) {
    {
        constexpr auto thing = sanisizer::attest_max_by_type<unsigned>(20);
        static_assert(std::is_same<decltype(thing), const int>::value);
        static_assert(thing == 20);
    }

    // Also works at run-time.
    {
        auto thing = sanisizer::attest_max_by_type<int>(i8(100));
        static_assert(std::is_same<decltype(thing), i8>::value);
        EXPECT_EQ(thing, 100);
    }

    // Same type is fine.
    {
        auto thing = sanisizer::attest_max_by_type<i32>(i32(2147483647));
        static_assert(std::is_same<decltype(thing), i32>::value);
        EXPECT_EQ(thing, 2147483647);
    }

    // Works when unsafe.
    {
        constexpr auto thing = sanisizer::set_bound_by_type_unsafe<u32>(u8(100));
        static_assert(std::is_same<decltype(thing), const u8>::value);
        static_assert(thing == 100);
    }
}

TEST(SetBoundByType, IntegerOp) {
    {
        constexpr auto thing = sanisizer::attest_max_by_type<i8>(u8(20));
        static_assert(sanisizer::is_Bounded<decltype(thing)>::value);
        static_assert(std::is_same<typename decltype(thing)::Integer, u8>::value);
        static_assert(sanisizer::get_value(thing) == 20);
        static_assert(sanisizer::get_max<decltype(thing)>() == 127);
    }

    // Also works at run-time.
    {
        auto thing = sanisizer::attest_max_by_type<u8>(10);
        static_assert(sanisizer::is_Bounded<decltype(thing)>::value);
        static_assert(std::is_same<typename decltype(thing)::Integer, int>::value);
        EXPECT_EQ(sanisizer::get_value(thing), 10);
        static_assert(sanisizer::get_max<decltype(thing)>() == 255);
    }

    // Again, with some larger types.
    {
        auto thing = sanisizer::attest_max_by_type<i32>(u32(1000));
        static_assert(sanisizer::is_Bounded<decltype(thing)>::value);
        static_assert(std::is_same<typename decltype(thing)::Integer, u32>::value);
        EXPECT_EQ(sanisizer::get_value(thing), 1000);
        static_assert(sanisizer::get_max<decltype(thing)>() == 2147483647);
    }

    // Works when unsafe.
    {
        constexpr auto thing = sanisizer::set_bound_by_type_unsafe<i8>(u32(100));
        static_assert(sanisizer::is_Bounded<decltype(thing)>::value);
        static_assert(std::is_same<typename decltype(thing)::Integer, u32>::value);
        EXPECT_EQ(sanisizer::get_value(thing), 100);
        static_assert(sanisizer::get_max<decltype(thing)>() == 127);
    }
}

TEST(SetBoundByType, IntegerError) {
    {
        bool failed = false;
        try {
            sanisizer::attest_max_by_type<i8>(128);
        } catch (std::domain_error&) {
            failed = true;
        }
        EXPECT_TRUE(failed);
    }

    // Again, unsigned.
    {
        bool failed = false;
        try {
            sanisizer::attest_max_by_type<u8>(1000u);
        } catch (std::domain_error&) {
            failed = true;
        }
        EXPECT_TRUE(failed);
    }
}

/**************************************/

TEST(SetBoundByType, BoundedNoOp) {
    {
        constexpr auto thing = sanisizer::attest_max_by_type<long>(sanisizer::Bounded<int, 20>(10));
        static_assert(thing.value == 10);
        static_assert(std::is_same<sanisizer::I<decltype(thing.value)>, int>::value);
        static_assert(decltype(thing)::max == 20);
    }

    // Works at runtime.
    {
        auto thing = sanisizer::attest_max_by_type<u8>(sanisizer::Bounded<i8, 120>(100));
        EXPECT_EQ(thing.value, 100);
        static_assert(std::is_same<decltype(thing.value), i8>::value);
        static_assert(decltype(thing)::max == 120);
    }

    // Same type is fine.
    {
        auto thing = sanisizer::attest_max_by_type<u32>(sanisizer::Bounded<u32, 2000000000>(1000000000));
        EXPECT_EQ(thing.value, 1000000000u);
        static_assert(std::is_same<decltype(thing.value), u32>::value);
        static_assert(decltype(thing)::max == 2000000000u);
    }

    // Works when unsafe.
    {
        constexpr auto thing = sanisizer::set_bound_by_type_unsafe<i32>(sanisizer::Bounded<u8, 200>(100));
        static_assert(sanisizer::is_Bounded<decltype(thing)>::value);
        static_assert(std::is_same<typename decltype(thing)::Integer, u8>::value);
        EXPECT_EQ(sanisizer::get_value(thing), 100);
        static_assert(sanisizer::get_max<decltype(thing)>() == 200);
    }
}

TEST(SetBoundByType, BoundedOp) {
    {
        constexpr auto thing = sanisizer::attest_max_by_type<u8>(sanisizer::Bounded<long, 2000>(10l));
        static_assert(sanisizer::is_Bounded<decltype(thing)>::value);
        static_assert(std::is_same<typename decltype(thing)::Integer, long>::value);
        static_assert(thing.value == 10);
        static_assert(decltype(thing)::max == 255);
    }

    // Also works at run-time.
    {
        auto thing = sanisizer::attest_max_by_type<u8>(sanisizer::Bounded<unsigned, 2000>(100u));
        static_assert(sanisizer::is_Bounded<decltype(thing)>::value);
        static_assert(std::is_same<typename decltype(thing)::Integer, unsigned>::value);
        EXPECT_EQ(sanisizer::get_value(thing), 100);
        static_assert(sanisizer::get_max<decltype(thing)>() == 255);
    }

    // Again, with some larger types.
    {
        auto thing = sanisizer::attest_max_by_type<i32>(sanisizer::Bounded<u32, 3000000000>(1000000000));
        static_assert(sanisizer::is_Bounded<decltype(thing)>::value);
        static_assert(std::is_same<typename decltype(thing)::Integer, u32>::value);
        EXPECT_EQ(sanisizer::get_value(thing), 1000000000);
        static_assert(sanisizer::get_max<decltype(thing)>() == 2147483647);
    }

    // Works when unsafe.
    {
        constexpr auto thing = sanisizer::set_bound_by_type_unsafe<i8>(sanisizer::Bounded<u8, 200>(100));
        static_assert(sanisizer::is_Bounded<decltype(thing)>::value);
        static_assert(std::is_same<typename decltype(thing)::Integer, u8>::value);
        EXPECT_EQ(sanisizer::get_value(thing), 100);
        static_assert(sanisizer::get_max<decltype(thing)>() == 127);
    }
}

TEST(SetBoundByType, BoundedError) {
    { 
        bool failed = false;
        try {
            sanisizer::set_bound_by_type<u8>(sanisizer::Attestation<i32, 2000>(1000));
        } catch (std::domain_error&) {
            failed = true;
        }
        EXPECT_TRUE(failed);
    }

    // Again, with different signs.
    { 
        bool failed = false;
        try {
            sanisizer::set_bound_by_type<i8>(sanisizer::Attestation<u32, 1000>(1000));
        } catch (std::domain_error&) {
            failed = true;
        }
        EXPECT_TRUE(failed);
    }
}

/**************************************/

TEST(SetBoundByType, ConstantNoOp) {
    {
        constexpr auto thing = sanisizer::attest_max_by_type<long>(sanisizer::Constant<int, 20>());
        static_assert(sanisizer::is_Constant<decltype(thing)>::value);
        static_assert(std::is_same<typename decltype(thing)::Integer, int>::value);
        static_assert(decltype(thing)::value == 20);
    }

    // Also works at run-time.
    {
        auto thing = sanisizer::attest_max_by_type<i8>(sanisizer::Constant<u8, 20>());
        static_assert(sanisizer::is_Constant<decltype(thing)>::value);
        static_assert(std::is_same<typename decltype(thing)::Integer, u8>::value);
        EXPECT_EQ(decltype(thing)::value, 20);
    }

    // Same type is fine.
    {
        auto thing = sanisizer::attest_max_by_type<i32>(sanisizer::Constant<i32, 1000000000>());
        static_assert(sanisizer::is_Constant<decltype(thing)>::value);
        static_assert(std::is_same<typename decltype(thing)::Integer, i32>::value);
        EXPECT_EQ(decltype(thing)::value, 1000000000);
    }

    // Same when unsafe.
    {
        auto thing = sanisizer::set_bound_by_type_unsafe<u32>(sanisizer::Constant<i32, 100000>());
        static_assert(sanisizer::is_Constant<decltype(thing)>::value);
        static_assert(std::is_same<typename decltype(thing)::Integer, i32>::value);
        EXPECT_EQ(decltype(thing)::value, 100000);
    }
}

TEST(SetBoundByType, ConstantError) {
    // We can only throw an error here, there is no path by which we return a Bounded when the input is a Constant.

    {
        bool failed = false;
        try {
            sanisizer::set_bound_by_type<u8>(sanisizer::Constant<int, 300>());
        } catch (std::domain_error&) {
            failed = true;
        }
        EXPECT_TRUE(failed);
    }

    {
        bool failed = false;
        try {
            sanisizer::set_bound_by_type<u32>(sanisizer::Constant<long long, 5000000000>());
        } catch (std::domain_error&) {
            failed = true;
        }
        EXPECT_TRUE(failed);
    }
}

/**************************************/

TEST(SetBound, Basic) {
    // Just getting some coverage on the original method.
    {
        constexpr auto thing = sanisizer::attest_max<std::int8_t, 99>(20u);
        static_assert(sanisizer::is_Bounded<decltype(thing)>::value);
        static_assert(std::is_same<typename decltype(thing)::Integer, unsigned>::value);
        static_assert(sanisizer::get_value(thing) == 20);
        static_assert(sanisizer::get_max<decltype(thing)>() == 99);
    }

    // Also works at run-time.
    {
        auto typed = sanisizer::attest_max<std::uint8_t, 33>(10);
        EXPECT_EQ(sanisizer::get_value(typed), 10);
        EXPECT_EQ(sanisizer::get_max<decltype(typed)>(), 33);
    }

    { 
        bool failed = false;
        try {
            sanisizer::attest_max<u8, 33>(34);
        } catch (std::domain_error& e) {
            failed = true;
        }
        EXPECT_TRUE(failed);
    }
}
