#include <gtest/gtest.h>
#include <cstddef>
#include <string>
#include <cstdint>

#include "sanisizer/ptrdiff.hpp"

struct MockIterator {
    MockIterator(std::size_t pos) : pos(pos) {}
    std::size_t pos;
};

std::uint8_t operator-(const MockIterator& left, const MockIterator& right) {
    return left.pos - right.pos;
}

TEST(CanPtrdiff, Basic) {
    EXPECT_EQ(MockIterator(10) - MockIterator(10), 0); // sanity check.

    {
        sanisizer::can_ptrdiff<MockIterator>(static_cast<std::int8_t>(15));
        sanisizer::can_ptrdiff<MockIterator>(15);

        bool failed = false;
        try {
            sanisizer::can_ptrdiff<MockIterator>(1000);
        } catch (sanisizer::OverflowError& e) {
            failed = true;
        }
        EXPECT_TRUE(failed);
    }

    // Also trying with arrays.
    {
        sanisizer::can_ptrdiff<double*>(static_cast<std::int8_t>(15));
        sanisizer::can_ptrdiff<double*>(15);

        bool failed = false;
        try {
            sanisizer::can_ptrdiff<MockIterator>(1000);
        } catch (sanisizer::OverflowError& e) {
            failed = true;
        }
        EXPECT_TRUE(failed);
    }
}
