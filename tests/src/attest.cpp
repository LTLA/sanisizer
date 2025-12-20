#include <gtest/gtest.h>

#include <type_traits>

#include "sanisizer/attest.hpp"

TEST(Attest, Class) {
    {
        constexpr sanisizer::Attestation<int, true, 20> val(5);
        static_assert(std::is_same<int, typename decltype(val)::Type>::value);
        static_assert(val.value == 5);
        static_assert(decltype(val)::limit == 20);
        static_assert(decltype(val)::is_unsigned);
    }

    {
        constexpr sanisizer::Attestation<unsigned, true, 100> val(10u);
        static_assert(std::is_same<unsigned, typename decltype(val)::Type>::value);
        static_assert(val.value == 10);
        static_assert(decltype(val)::limit == 100);
        static_assert(decltype(val)::is_unsigned);
    }

    {
        constexpr sanisizer::Attestation<long, false, 1000> val(20l);
        static_assert(std::is_same<long, typename decltype(val)::Type>::value);
        static_assert(val.value == 20);
        static_assert(decltype(val)::limit == 1000);
        static_assert(!decltype(val)::is_unsigned);
    }
}

TEST(Attest, GetValue) {
    static_assert(5 == sanisizer::get_value(sanisizer::Attestation<unsigned, true, 20>(5u)));
    static_assert(10 == sanisizer::get_value(sanisizer::Attestation<int, false, 20>(10)));
    static_assert(20 == sanisizer::get_value(sanisizer::Attestation<int, true, 20>(20)));
    static_assert(5 == sanisizer::get_value(5));
    static_assert(20 == sanisizer::get_value(20));
}

TEST(Attest, Unsigned) {
    {
        static_assert(sanisizer::is_unsigned(sanisizer::Attestation<unsigned, true, 20>(5u)));
        static_assert(!sanisizer::is_unsigned(sanisizer::Attestation<int, false, 20>(10)));
        static_assert(sanisizer::is_unsigned(sanisizer::Attestation<int, true, 20>(5)));
        static_assert(!sanisizer::is_unsigned(5));
        static_assert(sanisizer::is_unsigned(5u));
    }

    {
        constexpr auto thing = sanisizer::attest_unsigned(20u);
        static_assert(std::is_same<decltype(thing), const unsigned>::value);
    }

    {
        constexpr auto thing = sanisizer::attest_unsigned(20);
        static_assert(std::is_same<typename decltype(thing)::Type, int>::value);
        static_assert(thing.value == 20);
        static_assert(decltype(thing)::limit > 20);
        static_assert(decltype(thing)::is_unsigned);
    }

    {
        constexpr auto thing = sanisizer::attest_unsigned(sanisizer::Attestation<unsigned, true, 20>(10u));
        static_assert(std::is_same<typename decltype(thing)::Type, unsigned>::value);
        static_assert(thing.value == 10);
        static_assert(decltype(thing)::limit == 20);
        static_assert(decltype(thing)::is_unsigned);
    }

    {
        constexpr auto thing = sanisizer::attest_unsigned(sanisizer::Attestation<int, false, 20>(10));
        static_assert(std::is_same<typename decltype(thing)::Type, int>::value);
        static_assert(thing.value == 10);
        static_assert(decltype(thing)::limit == 20);
        static_assert(decltype(thing)::is_unsigned);
    }
}

TEST(Attest, Limit) {
    {
        static_assert(5 == sanisizer::get_limit(sanisizer::Attestation<unsigned, true, 5>(5u)));
        static_assert(10 == sanisizer::get_limit(sanisizer::Attestation<int, false, 10>(5)));
        static_assert(20 == sanisizer::get_limit(sanisizer::Attestation<int, true, 20>(5)));
        static_assert(sanisizer::get_limit(5) == std::numeric_limits<int>::max());
        static_assert(sanisizer::get_limit(5u) == std::numeric_limits<unsigned>::max());
    }

    {
        constexpr auto thing = sanisizer::attest_limit_by_type<char>(20u);
        static_assert(std::is_same<typename decltype(thing)::Type, unsigned>::value);
        static_assert(thing.value == 20);
        static_assert(sanisizer::get_limit(thing) == std::numeric_limits<char>::max());
        static_assert(decltype(thing)::is_unsigned);
    }

    {
        constexpr auto thing = sanisizer::attest_limit_by_type<unsigned>(20);
        static_assert(std::is_same<decltype(thing), const int>::value);
        static_assert(thing == 20);
    }

    {
        constexpr auto thing = sanisizer::attest_limit_by_type<char>(sanisizer::Attestation<int, true, 20>(10));
        static_assert(thing.value == 10);
        static_assert(decltype(thing)::limit == 20);
        static_assert(decltype(thing)::is_unsigned);
    }

    {
        constexpr auto thing = sanisizer::attest_limit_by_type<unsigned char>(sanisizer::Attestation<long, false, 2000>(10l));
        static_assert(thing.value == 10);
        static_assert(decltype(thing)::limit == std::numeric_limits<unsigned char>::max());
        static_assert(!decltype(thing)::is_unsigned);
    }
}

