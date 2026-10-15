#include <gtest/gtest.h>

#include <type_traits>
#include <cstdint>

#include "sanisizer/Bounded.hpp"

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
