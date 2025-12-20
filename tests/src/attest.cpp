#include <gtest/gtest.h>

#include <type_traits>

#include "sanisizer/attest.hpp"

TEST(Attest, Class) {
    {
        constexpr sanisizer::Attestation<int, true, 20> val(5);
        static_assert(std::is_same<int, typename decltype(val)::Type>::value);
        static_assert(val.value == 5);
        static_assert(decltype(val)::max == 20);
        static_assert(decltype(val)::gez);
    }

    {
        constexpr sanisizer::Attestation<unsigned, true, 100> val(10u);
        static_assert(std::is_same<unsigned, typename decltype(val)::Type>::value);
        static_assert(val.value == 10);
        static_assert(decltype(val)::max == 100);
        static_assert(decltype(val)::gez);
    }

    {
        constexpr sanisizer::Attestation<long, false, 1000> val(20l);
        static_assert(std::is_same<long, typename decltype(val)::Type>::value);
        static_assert(val.value == 20);
        static_assert(decltype(val)::max == 1000);
        static_assert(!decltype(val)::gez);
    }

    static_assert(sanisizer::is_Attestation<sanisizer::Attestation<int, true, 10> >::value);
    static_assert(sanisizer::is_integral_or_Attestation<sanisizer::Attestation<int, true, 10> >::value);
    static_assert(!sanisizer::is_Attestation<int>::value);
    static_assert(sanisizer::is_integral_or_Attestation<int>::value);
    static_assert(!sanisizer::is_integral_or_Attestation<char*>::value);
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
        static_assert(sanisizer::get_gez<sanisizer::Attestation<unsigned, true, 20> >());
        static_assert(!sanisizer::get_gez<sanisizer::Attestation<int, false, 20> >());
        static_assert(sanisizer::get_gez<sanisizer::Attestation<int, true, 20> >());
        static_assert(!sanisizer::get_gez<int>());
        static_assert(sanisizer::get_gez<unsigned>());
    }

    {
        constexpr auto thing = sanisizer::attest_gez(20u);
        static_assert(std::is_same<decltype(thing), const unsigned>::value);
    }

    {
        constexpr auto thing = sanisizer::attest_gez(20);
        static_assert(std::is_same<typename decltype(thing)::Type, int>::value);
        static_assert(thing.value == 20);
        static_assert(decltype(thing)::max > 20);
        static_assert(decltype(thing)::gez);
    }

    {
        constexpr auto thing = sanisizer::attest_gez(sanisizer::Attestation<unsigned, true, 20>(10u));
        static_assert(std::is_same<typename decltype(thing)::Type, unsigned>::value);
        static_assert(thing.value == 10);
        static_assert(decltype(thing)::max == 20);
        static_assert(decltype(thing)::gez);
    }

    {
        constexpr auto thing = sanisizer::attest_gez(sanisizer::Attestation<int, false, 20>(10));
        static_assert(std::is_same<typename decltype(thing)::Type, int>::value);
        static_assert(thing.value == 10);
        static_assert(decltype(thing)::max == 20);
        static_assert(decltype(thing)::gez);
    }
}

TEST(Attest, Limit) {
    {
        static_assert(5 == sanisizer::get_max<sanisizer::Attestation<unsigned, true, 5> >());
        static_assert(10 == sanisizer::get_max<sanisizer::Attestation<int, false, 10> >());
        static_assert(20 == sanisizer::get_max<sanisizer::Attestation<int, true, 20> >());
        static_assert(sanisizer::get_max<int>() == std::numeric_limits<int>::max());
        static_assert(sanisizer::get_max<unsigned>() == std::numeric_limits<unsigned>::max());
    }

    {
        constexpr auto thing = sanisizer::attest_max_by_type<char>(20u);
        static_assert(std::is_same<typename decltype(thing)::Type, unsigned>::value);
        static_assert(thing.value == 20);
        static_assert(decltype(thing)::max == std::numeric_limits<char>::max());
        static_assert(decltype(thing)::gez);
    }

    {
        constexpr auto thing = sanisizer::attest_max_by_type<unsigned>(20);
        static_assert(std::is_same<decltype(thing), const int>::value);
        static_assert(thing == 20);
    }

    {
        constexpr auto thing = sanisizer::attest_max_by_type<char>(sanisizer::Attestation<int, true, 20>(10));
        static_assert(thing.value == 10);
        static_assert(decltype(thing)::max == 20);
        static_assert(decltype(thing)::gez);
    }

    {
        constexpr auto thing = sanisizer::attest_max_by_type<unsigned char>(sanisizer::Attestation<long, false, 2000>(10l));
        static_assert(thing.value == 10);
        static_assert(decltype(thing)::max == std::numeric_limits<unsigned char>::max());
        static_assert(!decltype(thing)::gez);
    }
}

