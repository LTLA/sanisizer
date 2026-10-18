#include <gtest/gtest.h>

#include "sanisizer/nd_offset.hpp"
#include "sanisizer/comparisons.hpp"

#include <cstdint>

TEST(NdOffset, Integer) {
    EXPECT_EQ(sanisizer::nd_offset<int>(1, 2, 3), 7);
    EXPECT_EQ(sanisizer::nd_offset<int>(2, 0, 5), 2);
    EXPECT_EQ(sanisizer::nd_offset<int>(20, 10, 5), 70);
    EXPECT_EQ(sanisizer::nd_offset<int>(1, 2, 3, 4, 5), ((4 * 5) + 3) * 2 + 1);
    EXPECT_EQ(sanisizer::nd_offset<int>(1, 2, 3, 4, 5, 6, 7), (((6 * 7) + 5) * 4 + 3) * 2 + 1);

    // Works at compile time.
    static_assert(sanisizer::nd_offset<int>(1, 2, 3, 4, 5) == ((4 * 5) + 3) * 2 + 1);
}

TEST(NdOffset, Bounded) {
    EXPECT_EQ(sanisizer::nd_offset<int>(
        sanisizer::set_bound_by_type<std::uint8_t>(1),
        sanisizer::set_bound_by_type<std::uint8_t>(2),
        sanisizer::set_bound_by_type<std::uint8_t>(3)
    ), 7);

    EXPECT_EQ(sanisizer::nd_offset<int>(
        sanisizer::set_bound_by_type<std::uint8_t>(1),
        sanisizer::set_bound_by_type<std::uint8_t>(2),
        sanisizer::set_bound_by_type<std::uint8_t>(3),
        sanisizer::set_bound_by_type<std::uint8_t>(4),
        sanisizer::set_bound_by_type<std::uint8_t>(5)
    ), 1 + 2 * (3 + 4 * 5));

    // Works at compile time.
    static_assert(sanisizer::nd_offset<int>(
        sanisizer::set_bound_by_type<std::uint8_t>(1),
        sanisizer::set_bound_by_type<std::uint8_t>(2),
        sanisizer::set_bound_by_type<std::uint8_t>(3)
    ) == 7);
}

TEST(NdOffset, Constant) {
    EXPECT_EQ(sanisizer::nd_offset<int>(
        sanisizer::Constant<std::uint8_t, 1>(),
        sanisizer::Constant<std::uint8_t, 2>(),
        sanisizer::Constant<std::uint8_t, 3>()
    ), 7);

    EXPECT_EQ(sanisizer::nd_offset<int>(
        sanisizer::Constant<std::uint8_t, 1>(),
        sanisizer::Constant<std::uint8_t, 2>(),
        sanisizer::Constant<std::uint8_t, 3>(),
        sanisizer::Constant<std::uint8_t, 4>(),
        sanisizer::Constant<std::uint8_t, 5>()
    ), 1 + 2 * (3 + 4 * 5));

    // Works at compile time.
    static_assert(sanisizer::nd_offset<int>(
        sanisizer::Constant<std::uint8_t, 1>(),
        sanisizer::Constant<std::uint8_t, 2>(),
        sanisizer::Constant<std::uint8_t, 3>()
    ) == 7);
}
