#include <gtest/gtest.h>

#include <type_traits>
#include <cstdint>

#include "sanisizer/generics.hpp"

TEST(GetValue, Integer) {
    static_assert(5 == sanisizer::get_value(5));
    static_assert(20 == sanisizer::get_value(20));

    // Also works at runtime.
    EXPECT_EQ(sanisizer::get_value(5), 5);
}

TEST(GetValue, Bounded) {
    static_assert(5 == sanisizer::get_value(sanisizer::Bounded<unsigned, 20>(5u)));
    static_assert(10 == sanisizer::get_value(sanisizer::Bounded<int, 20>(10)));
    static_assert(20 == sanisizer::get_value(sanisizer::Bounded<int, 20>(20)));

    // also works at run-time.
    sanisizer::Bounded<int, 20> val(5);
    EXPECT_EQ(sanisizer::get_value(val), 5);
}

TEST(GetValue, Constant) {
    static_assert(5 == sanisizer::get_value(sanisizer::Constant<unsigned, 5>()));
    static_assert(10 == sanisizer::get_value(sanisizer::Constant<int, 10>()));
    static_assert(20 == sanisizer::get_value(sanisizer::Constant<long, 20>()));

    // also works at run-time.
    sanisizer::Constant<int, 20> val;
    EXPECT_EQ(sanisizer::get_value(val), 20);
}

/**************************************/

TEST(GetMax, Integer) {
    static_assert(sanisizer::get_max<int>() == std::numeric_limits<int>::max());
    static_assert(sanisizer::get_max<unsigned>() == std::numeric_limits<unsigned>::max());

    // Also works at runtime.
    EXPECT_EQ(sanisizer::get_max<char>(), std::numeric_limits<char>::max());
}

TEST(GetMax, Bounded) {
    static_assert(5 == sanisizer::get_max<sanisizer::Bounded<unsigned, 5> >());
    static_assert(10 == sanisizer::get_max<sanisizer::Bounded<int, 10> >());
    static_assert(20 == sanisizer::get_max<sanisizer::Bounded<long, 20> >());

    { // also works at run-time.
        sanisizer::Bounded<int, 20> val(5);
        EXPECT_EQ(sanisizer::get_max<decltype(val)>(), 20);
    }
}

TEST(GetMax, Constant) {
    static_assert(5 == sanisizer::get_max<sanisizer::Constant<unsigned, 5> >());
    static_assert(10 == sanisizer::get_max<sanisizer::Constant<int, 10> >());
    static_assert(20 == sanisizer::get_max<sanisizer::Constant<long, 20> >());

    { // also works at run-time.
        sanisizer::Constant<int, 20> val;
        EXPECT_EQ(sanisizer::get_max<decltype(val)>(), 20);
    }
}

/**************************************/

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

/**************************************/

TEST(CheckOverflow, Integer) {
    static_assert(!sanisizer::check_overflow<int>(10));
    static_assert(!sanisizer::check_overflow<int>(10u));
    static_assert(!sanisizer::check_overflow<unsigned>(10));
    static_assert(!sanisizer::check_overflow<unsigned>(10u));

    // Also works at run-time.
    {
        EXPECT_FALSE(sanisizer::check_overflow<int>(10));
        EXPECT_FALSE(sanisizer::check_overflow<int>(10u));
        EXPECT_FALSE(sanisizer::check_overflow<unsigned>(10));
        EXPECT_FALSE(sanisizer::check_overflow<unsigned>(10u));
    }

    {
        std::string msg;
        try {
            sanisizer::check_overflow<std::int8_t>(1000);
        } catch(std::overflow_error& e) { 
            msg = e.what();
        }
        EXPECT_TRUE(msg.find("overflow") != std::string::npos);
    }
}

TEST(CheckOverflow, Bounded) {
    static_assert(!sanisizer::check_overflow<int>(sanisizer::Bounded<int, 10>(10)));
    static_assert(!sanisizer::check_overflow<int>(sanisizer::Bounded<unsigned, 10>(10u)));
    static_assert(!sanisizer::check_overflow<unsigned>(sanisizer::Bounded<int, 10>(10)));
    static_assert(!sanisizer::check_overflow<unsigned>(sanisizer::Bounded<unsigned, 10>(10u)));

    // Also works at run-time.
    {
        EXPECT_FALSE(sanisizer::check_overflow<int>(sanisizer::Bounded<int, 10>(10)));
        EXPECT_FALSE(sanisizer::check_overflow<int>(sanisizer::Bounded<unsigned, 10>(10u)));
        EXPECT_FALSE(sanisizer::check_overflow<unsigned>(sanisizer::Bounded<int, 10>(10)));
        EXPECT_FALSE(sanisizer::check_overflow<unsigned>(sanisizer::Bounded<unsigned, 10>(10u)));
        EXPECT_FALSE(sanisizer::check_overflow<std::int8_t>(sanisizer::Bounded<int, 100>(10)));
        EXPECT_FALSE(sanisizer::check_overflow<std::int8_t>(sanisizer::Bounded<int, 200>(10)));
    }

    {
        std::string msg;
        try {
            sanisizer::check_overflow<std::int8_t>(sanisizer::Bounded<int, 2000>(1000));
        } catch(std::overflow_error& e) { 
            msg = e.what();
        }
        EXPECT_TRUE(msg.find("overflow") != std::string::npos);
    }
}

TEST(CheckOverflow, Constant) {
    static_assert(!sanisizer::check_overflow<int>(sanisizer::Constant<int, 10>()));
    static_assert(!sanisizer::check_overflow<int>(sanisizer::Constant<unsigned, 10>()));
    static_assert(!sanisizer::check_overflow<unsigned>(sanisizer::Constant<int, 10>()));
    static_assert(!sanisizer::check_overflow<unsigned>(sanisizer::Constant<unsigned, 10>()));

    // Also works at run-time.
    {
        EXPECT_FALSE(sanisizer::check_overflow<int>(sanisizer::Constant<int, 10>()));
        EXPECT_FALSE(sanisizer::check_overflow<int>(sanisizer::Constant<unsigned, 10>()));
        EXPECT_FALSE(sanisizer::check_overflow<unsigned>(sanisizer::Constant<int, 10>()));
        EXPECT_FALSE(sanisizer::check_overflow<unsigned>(sanisizer::Constant<unsigned, 10>()));
        EXPECT_FALSE(sanisizer::check_overflow<std::int8_t>(sanisizer::Constant<int, 100>()));
        EXPECT_FALSE(sanisizer::check_overflow<std::uint8_t>(sanisizer::Constant<int, 200>()));
    }

    {
        std::string msg;
        try {
            sanisizer::check_overflow<std::int8_t>(sanisizer::Constant<int, 2000>());
        } catch(std::overflow_error& e) { 
            msg = e.what();
        }
        EXPECT_TRUE(msg.find("overflow") != std::string::npos);
    }
}
