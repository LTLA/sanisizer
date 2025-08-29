#include <gtest/gtest.h>

#include "sanisizer/float.hpp"

#include <cstdint>

TEST(Float, FromFloat) {
    EXPECT_EQ(sanisizer::from_float<std::uint8_t>(0.0), 0);
    EXPECT_EQ(sanisizer::from_float<std::uint8_t>(6.0), 6);
    EXPECT_EQ(sanisizer::from_float<std::uint8_t>(100.0), 100);
    EXPECT_EQ(sanisizer::from_float<std::uint8_t>(255.5), 255);
    EXPECT_EQ(sanisizer::from_float<std::int16_t>(32767.9), 32767);

    std::string failmsg;
    try {
        std::cout << (int)sanisizer::from_float<std::uint8_t>(258.5) << std::endl;
    } catch (std::exception& e) {
        failmsg = e.what();
    }
    EXPECT_TRUE(failmsg.find("invalid conversion") != std::string::npos);

    failmsg.clear();
    try {
        std::cout << (int)sanisizer::from_float<std::int16_t>(32768.0) << std::endl;
    } catch (std::exception& e) {
        failmsg = e.what();
    }
    EXPECT_TRUE(failmsg.find("invalid conversion") != std::string::npos);

    failmsg.clear();
    try {
        sanisizer::from_float<std::uint8_t>(std::numeric_limits<double>::infinity());
    } catch (std::exception& e) {
        failmsg = e.what();
    }
    EXPECT_TRUE(failmsg.find("invalid conversion") != std::string::npos);
}
