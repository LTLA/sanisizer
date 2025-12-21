#include <gtest/gtest.h>

#include "sanisizer/comparisons.hpp"

#include <cstdint>

template<typename Fun_>
bool throws_on_negative(Fun_ fun) {
    bool failed = false;
    try {
        fun();
    } catch (std::out_of_range& e) {
        failed = true;
    }
    return failed;
}

TEST(Comparisons, Equal) {
    static_assert(sanisizer::is_equal(1, 1));
    static_assert(!sanisizer::is_equal(1, 0));
    static_assert(sanisizer::is_equal(1u, 1u));
    static_assert(!sanisizer::is_equal(1u, 0u));
    static_assert(sanisizer::is_equal(1, 1u));
    static_assert(!sanisizer::is_equal(1u, 0));

    EXPECT_TRUE(sanisizer::is_equal(10, 10u)); // for codecov.

    // Works with attestations.
    static_assert(!sanisizer::is_equal(sanisizer::Attestation<int, true, 100>(10), 20u));
    static_assert(sanisizer::is_equal(10u, sanisizer::Attestation<int, true, 100>(10)));
    static_assert(sanisizer::is_equal(sanisizer::Attestation<int, true, 100>(10), sanisizer::Attestation<unsigned, true, 20>(10u)));

    EXPECT_TRUE(throws_on_negative([]() -> void { sanisizer::is_equal(-1, 0); })); 
    EXPECT_TRUE(throws_on_negative([]() -> void { sanisizer::is_equal(0, -1); })); 
}

TEST(Comparisons, GreaterThan) {
    static_assert(!sanisizer::is_greater_than(1, 1));
    static_assert(sanisizer::is_greater_than(1, 0));
    static_assert(!sanisizer::is_greater_than(1u, 1u));
    static_assert(sanisizer::is_greater_than(1u, 0u));
    static_assert(!sanisizer::is_greater_than(1, 1u));
    static_assert(sanisizer::is_greater_than(1u, 0));

    EXPECT_TRUE(sanisizer::is_greater_than(11, 10u)); // for codecov.

    // Works with attestations.
    static_assert(!sanisizer::is_greater_than(sanisizer::Attestation<int, true, 100>(10), 20u));
    static_assert(sanisizer::is_greater_than(20u, sanisizer::Attestation<int, true, 100>(10)));
    static_assert(!sanisizer::is_greater_than(sanisizer::Attestation<int, true, 100>(10), sanisizer::Attestation<unsigned, true, 20>(10u)));

    EXPECT_TRUE(throws_on_negative([]() -> void { sanisizer::is_greater_than(-1, 0); })); 
    EXPECT_TRUE(throws_on_negative([]() -> void { sanisizer::is_greater_than(0, -1); })); 
}

TEST(Comparisons, LessThanOrEqual) {
    static_assert(sanisizer::is_less_than_or_equal(1, 1));
    static_assert(sanisizer::is_less_than_or_equal(0, 1));
    static_assert(!sanisizer::is_less_than_or_equal(1, 0));
    static_assert(sanisizer::is_less_than_or_equal(1u, 1u));
    static_assert(sanisizer::is_less_than_or_equal(0u, 1u));
    static_assert(!sanisizer::is_less_than_or_equal(1u, 0u));
    static_assert(sanisizer::is_less_than_or_equal(1, 1u));
    static_assert(sanisizer::is_less_than_or_equal(0, 1u));
    static_assert(!sanisizer::is_less_than_or_equal(1u, 0));

    EXPECT_TRUE(sanisizer::is_less_than_or_equal(9, 10u)); // for codecov.

    // Works with attestations.
    static_assert(sanisizer::is_less_than_or_equal(sanisizer::Attestation<int, true, 100>(10), 20u));
    static_assert(!sanisizer::is_less_than_or_equal(20u, sanisizer::Attestation<int, true, 100>(10)));
    static_assert(sanisizer::is_less_than_or_equal(sanisizer::Attestation<int, true, 100>(10), sanisizer::Attestation<unsigned, true, 20>(10u)));

    EXPECT_TRUE(throws_on_negative([]() -> void { sanisizer::is_less_than_or_equal(-1, 0); })); 
    EXPECT_TRUE(throws_on_negative([]() -> void { sanisizer::is_less_than_or_equal(0, -1); })); 
}

TEST(Comparisons, LessThan) {
    static_assert(sanisizer::is_less_than(0, 1));
    static_assert(!sanisizer::is_less_than(1, 1));
    static_assert(sanisizer::is_less_than(0u, 1u));
    static_assert(!sanisizer::is_less_than(1u, 1u));
    static_assert(sanisizer::is_less_than(0u, 1));
    static_assert(!sanisizer::is_less_than(1, 1u));

    EXPECT_TRUE(sanisizer::is_less_than(9, 10u)); // for codecov.

    // Works with attestations.
    static_assert(sanisizer::is_less_than(sanisizer::Attestation<int, true, 100>(10), 20u));
    static_assert(!sanisizer::is_less_than(20u, sanisizer::Attestation<int, true, 100>(10)));
    static_assert(!sanisizer::is_less_than(sanisizer::Attestation<int, true, 100>(10), sanisizer::Attestation<unsigned, true, 20>(10u)));

    EXPECT_TRUE(throws_on_negative([]() -> void { sanisizer::is_less_than(-1, 0); })); 
    EXPECT_TRUE(throws_on_negative([]() -> void { sanisizer::is_less_than(0, -1); })); 
}

TEST(Comparisons, GreaterThanOrEqual) {
    static_assert(sanisizer::is_greater_than_or_equal(1, 1));
    static_assert(sanisizer::is_greater_than_or_equal(1, 0));
    static_assert(!sanisizer::is_greater_than_or_equal(0, 1));
    static_assert(sanisizer::is_greater_than_or_equal(1u, 1u));
    static_assert(sanisizer::is_greater_than_or_equal(1u, 0u));
    static_assert(!sanisizer::is_greater_than_or_equal(0u, 1u));
    static_assert(sanisizer::is_greater_than_or_equal(1, 1u));
    static_assert(sanisizer::is_greater_than_or_equal(1, 0u));
    static_assert(!sanisizer::is_greater_than_or_equal(0u, 1));

    EXPECT_TRUE(sanisizer::is_greater_than_or_equal(11, 10u)); // for codecov.

    // Works with attestations.
    static_assert(!sanisizer::is_greater_than_or_equal(sanisizer::Attestation<int, true, 100>(10), 20u));
    static_assert(sanisizer::is_greater_than_or_equal(20u, sanisizer::Attestation<int, true, 100>(10)));
    static_assert(sanisizer::is_greater_than_or_equal(sanisizer::Attestation<int, true, 100>(10), sanisizer::Attestation<unsigned, true, 20>(10u)));

    EXPECT_TRUE(throws_on_negative([]() -> void { sanisizer::is_greater_than_or_equal(-1, 0); })); 
    EXPECT_TRUE(throws_on_negative([]() -> void { sanisizer::is_greater_than_or_equal(0, -1); })); 
}

TEST(Comparisons, Min) {
    auto x = sanisizer::min(static_cast<std::uint8_t>(255), 100);
    static_assert(std::is_same<decltype(x), std::uint8_t>::value);
    EXPECT_EQ(x, 100u);
    static_assert(sanisizer::min(static_cast<std::uint8_t>(255), 100) == 100u); // works at compile-time.

    auto x2 = sanisizer::min(static_cast<std::uint8_t>(255), 1000);
    static_assert(std::is_same<decltype(x2), std::uint8_t>::value);
    EXPECT_EQ(x2, 255u);
    static_assert(sanisizer::min(static_cast<std::uint8_t>(255), 1000) == 255u);

    auto x3 = sanisizer::min(500u, 1000);
    static_assert(std::is_same<decltype(x3), int>::value);
    EXPECT_EQ(x3, 500);
    static_assert(sanisizer::min(500u, 1000) == 500);

    auto x4 = sanisizer::min(10000u, 1000);
    static_assert(std::is_same<decltype(x4), int>::value);
    EXPECT_EQ(x4, 1000);
    static_assert(sanisizer::min(10000u, 1000) == 1000);

    // Check it works for attestations.
    constexpr auto attest_check1 = sanisizer::min(
        sanisizer::Attestation<std::int32_t, true, 10>((std::int32_t)10),
        sanisizer::Attestation<std::int8_t, false, 100>((std::int8_t)5)
    );
    static_assert(attest_check1 == 5);
    static_assert(std::is_same<decltype(attest_check1), const std::int32_t>::value);

    constexpr auto attest_check2 = sanisizer::min(
        sanisizer::Attestation<std::int8_t, true, 10>((std::int8_t)10),
        sanisizer::Attestation<std::int32_t, false, 100>((std::int32_t)7)
    );
    static_assert(attest_check2 == 7);
    static_assert(std::is_same<decltype(attest_check2), const std::int8_t>::value);

    EXPECT_TRUE(throws_on_negative([]() -> void { sanisizer::min(-1, 0); })); 
    EXPECT_TRUE(throws_on_negative([]() -> void { sanisizer::min(0, -1); })); 
}

TEST(Comparisons, Max) {
    auto x = sanisizer::max(static_cast<std::uint8_t>(255), 100);
    static_assert(std::is_same<decltype(x), int>::value);
    EXPECT_EQ(x, 255);
    static_assert(sanisizer::max(static_cast<std::uint8_t>(255), 100) == 255); // works at compile-time.

    auto x2 = sanisizer::max(static_cast<std::uint8_t>(255), 1000);
    static_assert(std::is_same<decltype(x2), int>::value);
    EXPECT_EQ(x2, 1000);
    static_assert(sanisizer::max(static_cast<std::uint8_t>(255), 1000) == 1000);

    auto x3 = sanisizer::max(500u, 1000);
    static_assert(std::is_same<decltype(x3), unsigned int>::value);
    EXPECT_EQ(x3, 1000u);
    static_assert(sanisizer::max(500u, 1000) == 1000u);

    auto x4 = sanisizer::max(10000u, 1000);
    static_assert(std::is_same<decltype(x4), unsigned int>::value);
    EXPECT_EQ(x4, 10000u);
    static_assert(sanisizer::max(10000u, 1000) == 10000u);

    // Check it works for attestations.
    constexpr auto attest_check1 = sanisizer::max(
        sanisizer::Attestation<std::int32_t, true, 10>((std::int32_t)10),
        sanisizer::Attestation<std::int8_t, false, 100>((std::int8_t)5)
    );
    static_assert(attest_check1 == 10);
    static_assert(std::is_same<decltype(attest_check1), const std::int8_t>::value);

    constexpr auto attest_check2 = sanisizer::max(
        sanisizer::Attestation<std::int8_t, true, 10>((std::int8_t)10),
        sanisizer::Attestation<std::int32_t, false, 100>((std::int32_t)7)
    );
    static_assert(attest_check2 == 10);
    static_assert(std::is_same<decltype(attest_check2), const std::int32_t>::value);

    EXPECT_TRUE(throws_on_negative([]() -> void { sanisizer::max(-1, 0); })); 
    EXPECT_TRUE(throws_on_negative([]() -> void { sanisizer::max(0, -1); })); 
}
