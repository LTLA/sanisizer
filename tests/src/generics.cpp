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

