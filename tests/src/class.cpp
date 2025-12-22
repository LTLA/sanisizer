#include <gtest/gtest.h>

#include "sanisizer/class.hpp"

#include <cstdint>

static constexpr auto check_value(std::uint8_t x) {
    return x;
}

TEST(Class, Cast) {
    EXPECT_EQ(check_value(sanisizer::Cast(2)), 2);
    static_assert(check_value(sanisizer::Cast(2)) == 2);

    EXPECT_EQ(check_value(sanisizer::Cast(sanisizer::attest_gez(2))), 2);
    static_assert(check_value(sanisizer::Cast(sanisizer::attest_gez(2))) == 2);

    bool failed = false;
    try {
        check_value(sanisizer::Cast(256));
    } catch (std::overflow_error& e) {
        failed = true;
    }
    EXPECT_TRUE(failed);
}

TEST(Class, Exact) {
    EXPECT_EQ(check_value(sanisizer::Exact(static_cast<std::uint8_t>(2))), 2);
    static_assert(check_value(sanisizer::Exact(static_cast<std::uint8_t>(2))) == 2);
}
