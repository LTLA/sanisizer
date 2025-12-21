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
    EXPECT_TRUE(failmsg.find("overflow detected") != std::string::npos);

    failmsg.clear();
    try {
        std::cout << (int)sanisizer::from_float<std::int16_t>(32768.0) << std::endl;
    } catch (std::exception& e) {
        failmsg = e.what();
    }
    EXPECT_TRUE(failmsg.find("overflow detected") != std::string::npos);

    failmsg.clear();
    try {
        sanisizer::from_float<std::uint8_t>(std::numeric_limits<double>::infinity());
    } catch (std::exception& e) {
        failmsg = e.what();
    }
    EXPECT_TRUE(failmsg.find("invalid conversion") != std::string::npos);

    failmsg.clear();
    try {
        sanisizer::from_float<std::uint8_t>(-1.0);
    } catch (std::exception& e) {
        failmsg = e.what();
    }
    EXPECT_TRUE(failmsg.find("negative input value") != std::string::npos);
}

TEST(Float, ToFloat) {
    EXPECT_EQ(sanisizer::to_float<float>(0), 0.0);
    EXPECT_EQ(sanisizer::to_float<float>(6), 6.0);
    EXPECT_EQ(sanisizer::to_float<float>(100), 100.0);
    EXPECT_EQ(sanisizer::to_float<float>(255), 255.0);
    EXPECT_EQ(sanisizer::to_float<float>(32767), 32767.0);

    {
        std::string failmsg;
        try {
            std::cout << sanisizer::to_float<float>(-1) << std::endl;
        } catch (std::exception& e) {
            failmsg = e.what();
        }
        EXPECT_TRUE(failmsg.find("negative") != std::string::npos);
    }

    if constexpr(std::numeric_limits<float>::is_iec559) {
        EXPECT_EQ(sanisizer::to_float<float>(16777216), 16777216.0);
        EXPECT_EQ(sanisizer::to_float<double>(9007199254740992), 9007199254740992.0); 

        std::string failmsg;
        try {
            std::cout << sanisizer::to_float<float>(16777217) << std::endl;
        } catch (std::exception& e) {
            failmsg = e.what();
        }
        EXPECT_TRUE(failmsg.find("overflow detected") != std::string::npos);

        failmsg.clear();
        try {
            std::cout << sanisizer::to_float<double>(9007199254740993) << std::endl;
        } catch (std::exception& e) {
            failmsg = e.what();
        }
        EXPECT_TRUE(failmsg.find("overflow detected") != std::string::npos);
    }

    // Works with attestations.
    EXPECT_EQ(sanisizer::to_float<double>(sanisizer::Attestation<std::int64_t, true, 0>((std::int64_t)0)), 0.0);
    EXPECT_EQ(sanisizer::to_float<double>(sanisizer::Attestation<std::int64_t, true, 100>((std::int64_t)10)), 10.0);
    EXPECT_EQ(sanisizer::to_float<double>(sanisizer::Attestation<std::int64_t, true, std::numeric_limits<std::int64_t>::max()>((std::int64_t)10)), 10.0);
}
