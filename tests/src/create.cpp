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

    // Check that it throws the right errors.
    struct MockVector {
        MockVector(std::uint8_t) {}
        std::uint8_t size() const { return 0; }
    };
    bool failed = false;
    try {
        sanisizer::create<MockVector>(300);
    } catch (std::overflow_error& e) {
        failed = true;
    }
    EXPECT_TRUE(failed);

    failed = false;
    try {
        sanisizer::create<std::vector<int> >(-1);
    } catch (std::out_of_range& e) {
        failed = true;
    }
    EXPECT_TRUE(failed);

    // Check that it works with attestations.
    auto output_attest = sanisizer::create<std::vector<int> >(sanisizer::Attestation<int, true, 20>(10));
    EXPECT_EQ(output_attest.size(), 10);
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
    } catch (std::overflow_error& e) {
        failed = true;
    }
    EXPECT_TRUE(failed);
}
