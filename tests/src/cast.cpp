#include <gtest/gtest.h>

#include "sanisizer/cast.hpp"

#include <cstdint>

TEST(Cast, Basic) {
    EXPECT_EQ(sanisizer::cast<unsigned>(1u), 1u);
    EXPECT_EQ(sanisizer::cast<int>(1), 1);
    EXPECT_EQ(sanisizer::cast<unsigned>(1), 1u);
    EXPECT_EQ(sanisizer::cast<int>(1u), 1);

    EXPECT_EQ(sanisizer::cast<std::uint8_t>(1u), 1u);
    EXPECT_EQ(sanisizer::cast<unsigned>(static_cast<std::uint8_t>(1)), 1);
    EXPECT_EQ(sanisizer::cast<std::uint8_t>(1u), 1u);
    EXPECT_EQ(sanisizer::cast<unsigned>(static_cast<std::uint8_t>(1)), 1);

    bool failed = false;
    try {
        sanisizer::cast<std::uint8_t>(256);
    } catch (sanisizer::OverflowError& e) {
        failed = true;
    }
    EXPECT_TRUE(failed);
}

TEST(CanCast, Basic) {
    EXPECT_EQ(sanisizer::can_cast<unsigned>(1u), 1u);
    EXPECT_EQ(sanisizer::can_cast<int>(1), 1);
    EXPECT_EQ(sanisizer::can_cast<unsigned>(1), 1u);
    EXPECT_EQ(sanisizer::can_cast<int>(1u), 1);

    EXPECT_EQ(sanisizer::can_cast<std::uint8_t>(1u), 1u);
    EXPECT_EQ(sanisizer::can_cast<unsigned>(static_cast<std::uint8_t>(1)), 1);
    EXPECT_EQ(sanisizer::can_cast<std::uint8_t>(1u), 1u);
    EXPECT_EQ(sanisizer::can_cast<unsigned>(static_cast<std::uint8_t>(1)), 1);

    bool failed = false;
    try {
        sanisizer::can_cast<std::uint8_t>(256);
    } catch (sanisizer::OverflowError& e) {
        failed = true;
    }
    EXPECT_TRUE(failed);
}
