#include <gtest/gtest.h>

#include "sanisizer/create.hpp"

#include <cstdint>

TEST(Create, Basic) {
    auto output = sanisizer::create<std::vector<int> >(20);
    EXPECT_EQ(output.size(), 20);

    auto output2 = sanisizer::create<std::vector<int> >(20, 1);
    EXPECT_EQ(output2.size(), 20);
    EXPECT_EQ(output2.front(), 1);
    EXPECT_EQ(output2.back(), 1);

    struct MockVector {
        MockVector(std::uint8_t) {}
        std::uint8_t size() const { return 0; }
    };
    bool failed = false;
    try {
        sanisizer::create<MockVector>(300);
    } catch (sanisizer::OverflowError& e) {
        failed = true;
    }
    EXPECT_TRUE(failed);
}

TEST(Resize, Basic) {
    std::vector<int> output(20);
    sanisizer::resize(output, 10);
    EXPECT_EQ(output.size(), 10);

    sanisizer::resize(output, 20, 1);
    EXPECT_EQ(output.size(), 20);
    EXPECT_EQ(output.front(), 0);
    EXPECT_EQ(output[9], 0);
    EXPECT_EQ(output[10], 1);
    EXPECT_EQ(output.back(), 1);

    struct MockVector {
        std::uint8_t size() const { return 0; }
        void resize(std::uint8_t) {}
    };

    bool failed = false;
    MockVector foo;
    try {
        sanisizer::resize(foo, 300);
    } catch (sanisizer::OverflowError& e) {
        failed = true;
    }
    EXPECT_TRUE(failed);
}
