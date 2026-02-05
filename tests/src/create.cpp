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

    bool failed = false;
    try {
        sanisizer::create<std::vector<int> >(-1);
    } catch (std::out_of_range& e) {
        failed = true;
    }
    EXPECT_TRUE(failed);

    // Check that it works with attestations.
    auto output_attest = sanisizer::create<std::vector<int> >(sanisizer::Attestation<int, true, 20>(10));
    EXPECT_EQ(output_attest.size(), 10);

    // as_size_type works at compile-time.
    static_assert(sanisizer::as_size_type<std::vector<int> >(10) == 10);
}

struct MockVector {
    MockVector() = default;
    MockVector(std::uint8_t) {}
    std::uint8_t size() const { return 0; }
    void resize(std::uint8_t) const {}
    void reserve(std::uint8_t) const {}
};

TEST(Create, MockSize) {
    auto out = sanisizer::create<MockVector>(200);
    EXPECT_EQ(out.size(), 0);

    bool failed = false;
    try {
        sanisizer::create<MockVector>(300);
    } catch (std::overflow_error& e) {
        failed = true;
    }
    EXPECT_TRUE(failed);
}

TEST(Create, MockPtrDiff) {
    struct MockVector2 {
        MockVector2(std::uint8_t) {}
        std::size_t size() const { return 0; }

        struct Ptr {
            std::int8_t operator-(const Ptr&) { return 0; }
        };
        Ptr begin() const { return Ptr(); }
        Ptr end() const { return Ptr(); }
    };

    auto out = sanisizer::create<MockVector2>(100);
    EXPECT_EQ(out.size(), 0);

    bool failed = false;
    try {
        sanisizer::create<MockVector2>(200);
    } catch (std::overflow_error& e) {
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

    MockVector foo;
    sanisizer::resize(foo, 1);

    bool failed = false;
    try {
        sanisizer::resize(foo, 300);
    } catch (std::overflow_error& e) {
        failed = true;
    }
    EXPECT_TRUE(failed);
}

TEST(Reserve, Basic) {
    std::vector<int> output;
    sanisizer::reserve(output, 10);
    EXPECT_GE(output.capacity(), 10);

    MockVector foo;
    sanisizer::reserve(foo, 1);

    bool failed = false;
    try {
        sanisizer::reserve(foo, 300);
    } catch (std::overflow_error& e) {
        failed = true;
    }
    EXPECT_TRUE(failed);
}
