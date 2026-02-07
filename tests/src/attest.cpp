#include <gtest/gtest.h>

#include <type_traits>
#include <cstdint>

#include "sanisizer/attest.hpp"

TEST(Attest, Class) {
    {
        constexpr sanisizer::Attestation<int, 20> val(5);
        static_assert(std::is_same<int, typename decltype(val)::Integer>::value);
        static_assert(val.value == 5);
        static_assert(decltype(val)::max == 20);
    }

    {
        constexpr sanisizer::Attestation<unsigned, 100> val(10u);
        static_assert(std::is_same<unsigned, typename decltype(val)::Integer>::value);
        static_assert(val.value == 10);
        static_assert(decltype(val)::max == 100);
    }

    {
        constexpr sanisizer::Attestation<long, 1000> val(20l);
        static_assert(std::is_same<long, typename decltype(val)::Integer>::value);
        static_assert(val.value == 20);
        static_assert(decltype(val)::max == 1000);
    }

    { // also works at run-time.
        sanisizer::Attestation<int, 20> val(5);
        EXPECT_EQ(val.value, 5);
    }

    static_assert(sanisizer::is_Attestation<sanisizer::Attestation<int, 10> >::value);
    static_assert(sanisizer::is_integral_or_Attestation<sanisizer::Attestation<int, 10> >::value);
    static_assert(!sanisizer::is_Attestation<int>::value);
    static_assert(sanisizer::is_integral_or_Attestation<int>::value);
    static_assert(!sanisizer::is_integral_or_Attestation<char*>::value);
}

TEST(Attest, GetValue) {
    static_assert(5 == sanisizer::get_value(sanisizer::Attestation<unsigned, 20>(5u)));
    static_assert(10 == sanisizer::get_value(sanisizer::Attestation<int, 20>(10)));
    static_assert(20 == sanisizer::get_value(sanisizer::Attestation<int, 20>(20)));
    static_assert(5 == sanisizer::get_value(5));
    static_assert(20 == sanisizer::get_value(20));

    { // also works at run-time.
        sanisizer::Attestation<int, 20> val(5);
        EXPECT_EQ(sanisizer::get_value(val), 5);
        EXPECT_EQ(sanisizer::get_value(5), 5);
    }
}

TEST(Attest, Limit) {
    {
        static_assert(5 == sanisizer::get_max<sanisizer::Attestation<unsigned, 5> >());
        static_assert(10 == sanisizer::get_max<sanisizer::Attestation<int, 10> >());
        static_assert(20 == sanisizer::get_max<sanisizer::Attestation<int, 20> >());
        static_assert(sanisizer::get_max<int>() == std::numeric_limits<int>::max());
        static_assert(sanisizer::get_max<unsigned>() == std::numeric_limits<unsigned>::max());
    }

    {
        constexpr auto thing = sanisizer::attest_max_by_type<std::int8_t>(20u);
        static_assert(std::is_same<typename decltype(thing)::Integer, unsigned>::value);
        static_assert(thing.value == 20);
        static_assert(decltype(thing)::max == std::numeric_limits<std::int8_t>::max());
    }

    {
        constexpr auto thing = sanisizer::attest_max_by_type<unsigned>(20);
        static_assert(std::is_same<decltype(thing), const int>::value);
        static_assert(thing == 20);
    }

    {
        constexpr auto thing = sanisizer::attest_max_by_type<std::int8_t>(sanisizer::Attestation<int, 20>(10));
        static_assert(thing.value == 10);
        static_assert(decltype(thing)::max == 20);
    }

    {
        constexpr auto thing = sanisizer::attest_max_by_type<std::uint8_t>(sanisizer::Attestation<long, 2000>(10l));
        static_assert(thing.value == 10);
        static_assert(decltype(thing)::max == std::numeric_limits<std::uint8_t>::max());
    }

    { // also works at run-time.
        sanisizer::Attestation<int, 20> val(5);
        EXPECT_EQ(sanisizer::get_max<decltype(val)>(), 20);

        auto copy = sanisizer::attest_max<int, 10>(val);
        EXPECT_EQ(sanisizer::get_max<decltype(copy)>(), 10);
        auto copy2 = sanisizer::attest_max<int, 100>(val);
        EXPECT_EQ(sanisizer::get_max<decltype(copy2)>(), 20);

        auto imax = sanisizer::attest_max<int, 10>(10);
        EXPECT_EQ(sanisizer::get_max<decltype(imax)>(), 10);
        auto imax2 = sanisizer::attest_max<int, 1000>((std::uint8_t)10);
        EXPECT_EQ(sanisizer::get_max<decltype(imax2)>(), 255);

        auto typed = sanisizer::attest_max_by_type<std::uint8_t>(10);
        EXPECT_EQ(sanisizer::get_max<decltype(typed)>(), 255);
    }
}

TEST(Attest, CheckOverflow) {
    {
        static_assert(!sanisizer::check_overflow<int>(10));
        static_assert(!sanisizer::check_overflow<int>(10u));
        static_assert(!sanisizer::check_overflow<int>(sanisizer::Attestation<int, 10>(10)));
        static_assert(!sanisizer::check_overflow<int>(sanisizer::Attestation<unsigned, 10>(10u)));

        static_assert(!sanisizer::check_overflow<unsigned>(10));
        static_assert(!sanisizer::check_overflow<unsigned>(10u));
        static_assert(!sanisizer::check_overflow<unsigned>(sanisizer::Attestation<int, 10>(10)));
        static_assert(!sanisizer::check_overflow<unsigned>(sanisizer::Attestation<unsigned, 10>(10u)));
    }

    // Also works at run-time.
    {
        EXPECT_FALSE(sanisizer::check_overflow<int>(10));
        EXPECT_FALSE(sanisizer::check_overflow<int>(10u));
        EXPECT_FALSE(sanisizer::check_overflow<int>(sanisizer::Attestation<int, 10>(10)));
        EXPECT_FALSE(sanisizer::check_overflow<int>(sanisizer::Attestation<unsigned, 10>(10u)));

        EXPECT_FALSE(sanisizer::check_overflow<unsigned>(10));
        EXPECT_FALSE(sanisizer::check_overflow<unsigned>(10u));
        EXPECT_FALSE(sanisizer::check_overflow<unsigned>(sanisizer::Attestation<int, 10>(10)));
        EXPECT_FALSE(sanisizer::check_overflow<unsigned>(sanisizer::Attestation<unsigned, 10>(10u)));

        EXPECT_FALSE(sanisizer::check_overflow<std::int8_t>(sanisizer::Attestation<int, 100>(10)));
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
