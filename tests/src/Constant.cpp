#include <gtest/gtest.h>

#include <type_traits>
#include <cstdint>

#include "sanisizer/Constant.hpp"

TEST(Constant, CompileTime) {
    {
        constexpr sanisizer::Constant<int, 20> val;
        static_assert(std::is_same<int, typename decltype(val)::Integer>::value);
        static_assert(decltype(val)::value == 20);
    }

    {
        constexpr sanisizer::Constant<unsigned, 100> val;
        static_assert(std::is_same<unsigned, typename decltype(val)::Integer>::value);
        static_assert(decltype(val)::value == 100);
    }

    {
        constexpr sanisizer::Constant<long, 1000> val;
        static_assert(std::is_same<long, typename decltype(val)::Integer>::value);
        static_assert(decltype(val)::value == 1000);
    }

    static_assert(sanisizer::is_Constant<sanisizer::Constant<int, 10> >::value);
    static_assert(sanisizer::is_Constant<const sanisizer::Constant<int, 10> >::value);
    static_assert(!sanisizer::is_Constant<int>::value);
}

TEST(Constant, RunTime) {
    sanisizer::Constant<int, 20> val;
    EXPECT_EQ(decltype(val)::value, 20);
}
