#include <gtest/gtest.h>

#include "sanisizer/float.hpp"

#include <cstdint>

TEST(Float, RequiredBits) {
    EXPECT_EQ(sanisizer::required_bits_for_float(0.), 0);
    EXPECT_EQ(sanisizer::required_bits_for_float(1.), 1);
    EXPECT_EQ(sanisizer::required_bits_for_float(2.), 2);
    EXPECT_EQ(sanisizer::required_bits_for_float(3.), 2);
    EXPECT_EQ(sanisizer::required_bits_for_float(4.), 3);
    EXPECT_EQ(sanisizer::required_bits_for_float(10.), 4);
    EXPECT_EQ(sanisizer::required_bits_for_float(25.), 5);
    EXPECT_EQ(sanisizer::required_bits_for_float(50.), 6);
    EXPECT_EQ(sanisizer::required_bits_for_float(100.), 7);

    EXPECT_EQ(sanisizer::required_bits_for_float(127.), 7);
    EXPECT_EQ(sanisizer::required_bits_for_float(128.), 8);
    EXPECT_EQ(sanisizer::required_bits_for_float(255.), 8);
    EXPECT_EQ(sanisizer::required_bits_for_float(256.), 9);

    EXPECT_EQ(sanisizer::required_bits_for_float(32767.), 15);
    EXPECT_EQ(sanisizer::required_bits_for_float(32768.), 16);
    EXPECT_EQ(sanisizer::required_bits_for_float(65535.), 16);
    EXPECT_EQ(sanisizer::required_bits_for_float(65536.), 17);

    EXPECT_EQ(sanisizer::required_bits_for_float(2147483647.0), 31);
    EXPECT_EQ(sanisizer::required_bits_for_float(2147483648.0), 32);
    EXPECT_EQ(sanisizer::required_bits_for_float(4294967295.0), 32);
    EXPECT_EQ(sanisizer::required_bits_for_float(4294967296.0), 33);
}

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
    EXPECT_EQ(sanisizer::to_float<double>(sanisizer::Attestation<std::int64_t, 0>((std::int64_t)0)), 0.0);
    EXPECT_EQ(sanisizer::to_float<double>(sanisizer::Attestation<std::int64_t, 100>((std::int64_t)10)), 10.0);
    EXPECT_EQ(sanisizer::to_float<double>(sanisizer::Attestation<std::int64_t, std::numeric_limits<std::int64_t>::max()>((std::int64_t)10)), 10.0);
}
