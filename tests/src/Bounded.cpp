#include <gtest/gtest.h>

#include <type_traits>
#include <cstdint>

#include "sanisizer/Bounded.hpp"
#include "sanisizer/generics.hpp"

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

TEST(SetBoundByType, IntegerNoOp) {
    {
        constexpr auto thing = sanisizer::attest_max_by_type<unsigned>(20);
        static_assert(std::is_same<decltype(thing), const int>::value);
        static_assert(thing == 20);
    }

    // Also works at run-time.
    EXPECT_EQ(sanisizer::attest_max_by_type<unsigned>(10), 10);
}

TEST(SetBoundByType, BoundedNoOp) {
    {
        constexpr auto thing = sanisizer::attest_max_by_type<long>(sanisizer::Bounded<int, 20>(10));
        static_assert(thing.value == 10);
        static_assert(decltype(thing)::max == 20);
    }

    // Also works at run-time.
    {
        auto val = sanisizer::attest_max_by_type<long>(sanisizer::Bounded<int, 20>(10));
        EXPECT_EQ(val.value, 10);
        EXPECT_EQ(decltype(val)::max, 20);
    }
}

TEST(SetBoundByType, ConstantNoOp) {
    {
        constexpr auto thing = sanisizer::attest_max_by_type<long>(sanisizer::Constant<int, 20>());
        static_assert(decltype(thing)::value == 20);
    }

    // Also works at run-time.
    {
        auto val = sanisizer::attest_max_by_type<long>(sanisizer::Constant<int, 20>());
        EXPECT_EQ(decltype(val)::value, 20);
    }
}

TEST(SetBoundByType, IntegerOp) {
    {
        constexpr auto thing = sanisizer::attest_max_by_type<std::int8_t>(20u);
        static_assert(sanisizer::is_Bounded<decltype(thing)>::value);
        static_assert(std::is_same<typename decltype(thing)::Integer, unsigned>::value);
        static_assert(sanisizer::get_value(thing) == 20);
        static_assert(sanisizer::get_max<decltype(thing)>() == 127);
    }

    // Also works at run-time.
    {
        auto typed = sanisizer::attest_max_by_type<std::uint8_t>(10);
        EXPECT_EQ(sanisizer::get_value(typed), 10);
        EXPECT_EQ(sanisizer::get_max<decltype(typed)>(), 255);
    }

    // Throws an error.
    { 
        bool failed = false;
        try {
            sanisizer::attest_max<std::uint8_t, 33>(34);
        } catch (std::domain_error& e) {
            failed = true;
        }
        EXPECT_TRUE(failed);
    }
}

TEST(SetBoundByType, BoundedOp) {
    {
        constexpr auto thing = sanisizer::attest_max_by_type<std::uint8_t>(sanisizer::Bounded<long, 2000>(10l));
        static_assert(sanisizer::is_Bounded<decltype(thing)>::value);
        static_assert(std::is_same<typename decltype(thing)::Integer, long>::value);
        static_assert(thing.value == 10);
        static_assert(decltype(thing)::max == 255);
    }

    // Also works at run-time.
    {
        auto thing = sanisizer::attest_max_by_type<std::uint8_t>(sanisizer::Bounded<long, 2000>(10l));
        EXPECT_EQ(sanisizer::get_value(thing), 10);
        EXPECT_EQ(sanisizer::get_max<decltype(thing)>(), 255);
    }

    // Throws an error.
    { 
        bool failed = false;
        try {
            sanisizer::attest_max<std::uint8_t, 33>(sanisizer::Bounded<int, 100>(34));
        } catch (std::domain_error& e) {
            failed = true;
        }
        EXPECT_TRUE(failed);
    }
}

TEST(SetBoundByType, ConstantOp) {
    // This can only throw an error.
    bool failed = false;
    try {
        sanisizer::attest_max<std::uint8_t, 33>(sanisizer::Constant<int, 34>());
    } catch (std::domain_error& e) {
        failed = true;
    }
    EXPECT_TRUE(failed);
}

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
}
