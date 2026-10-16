#include <gtest/gtest.h>

#include <type_traits>
#include <cstdint>
#include <stdexcept>

#include "sanisizer/overflow.hpp"

TEST(CheckOverflow, Integer) {
    static_assert(!sanisizer::check_overflow<int>(10));
    static_assert(!sanisizer::check_overflow<int>(10u));
    static_assert(!sanisizer::check_overflow<unsigned>(10));
    static_assert(!sanisizer::check_overflow<unsigned>(10u));

    static_assert(sanisizer::does_overflow<std::int8_t>(128));
    static_assert(sanisizer::does_overflow<std::int8_t>(128u));
    static_assert(sanisizer::does_overflow<std::uint8_t>(256));
    static_assert(sanisizer::does_overflow<std::uint8_t>(256u));

    // Also works at run-time.
    {
        EXPECT_FALSE(sanisizer::check_overflow<int>(10));
        EXPECT_FALSE(sanisizer::check_overflow<int>(10u));
        EXPECT_FALSE(sanisizer::check_overflow<unsigned>(10));
        EXPECT_FALSE(sanisizer::check_overflow<unsigned>(10u));

        EXPECT_TRUE(sanisizer::does_overflow<std::int8_t>(128));
        EXPECT_TRUE(sanisizer::does_overflow<std::int8_t>(128u));
        EXPECT_TRUE(sanisizer::does_overflow<std::uint8_t>(256));
        EXPECT_TRUE(sanisizer::does_overflow<std::uint8_t>(256u));
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
