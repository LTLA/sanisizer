#include <gtest/gtest.h>

#include "sanisizer/cap.hpp"

#include <cstdint>

TEST(Cap, Basic) {
    EXPECT_EQ(sanisizer::cap<std::uint8_t>(1), 1);
    EXPECT_EQ(sanisizer::cap<std::uint8_t>(100), 100);
    EXPECT_EQ(sanisizer::cap<std::uint8_t>(1000), 255);

    bool okay = false;
    if constexpr(sanisizer::cap<std::uint8_t>(1000) == 255) {
        okay = true;
    }
    EXPECT_TRUE(okay);
}

struct FooBarOptions {
    std::int8_t thing8 = sanisizer::cap<std::int8_t>(10000);
    std::int16_t thing16 = sanisizer::cap<std::int16_t>(10000);
};

TEST(Cap, Struct) {
    FooBarOptions opt;
    EXPECT_EQ(opt.thing8, 127);
    EXPECT_EQ(opt.thing16, 10000);
}

template<typename T>
T foobar_func(T val = sanisizer::cap<T>(10000)) {
    return val;
}

TEST(Cap, Func) {
    EXPECT_EQ(foobar_func<std::int8_t>(), 127);
    EXPECT_EQ(foobar_func<std::int16_t>(), 10000);
}
