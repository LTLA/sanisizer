#ifndef SANISIZER_CONSTANT_HPP
#define SANISIZER_CONSTANT_HPP

#include <type_traits>

/**
 * @file Constant.hpp
 * @brief Specify a constant integer.
 */

namespace sanisizer {

/**
 * @brief Integer constant.
 *
 * @tparam Integer_ Type of the integer.
 * @tparam value_ Value of the integer, known at compile time.
 * This should be non-negative.
 *
 * The aim is to provide a mechanism to detect a compile-time constant, allowing developers to reorganize their calculations to push more work to compile time.
 * This enables some optimizations in functions like `sum()` and `product()`.
 */
template<typename Integer_, Integer_ value_>
struct Constant {
    static_assert(std::is_integral<Integer_>::value);
    static_assert(value_ >= 0);

    /**
     * Type of the integer.
     */
    typedef Integer_ Integer;

    /**
     * Value of the constant integer, known at compile time.
     * This will be non-negative.
     */
    static constexpr Integer_ value = value_;
};

/**
 * Default case for checking whether `Value_` is a `Constant`.
 *
 * @tparam Value_ Type to be queried.
 */
template<typename Value_>
struct is_Constant {
    /**
     * Fale, `Value_` is not a `Constant`.
     */
    static constexpr bool value = false;
};

/**
 * Positive case for checking whether a class instace is a `Constant`.
 *
 * @tparam Integer_ See documentation for `Constant`.
 * @tparam value_ See documentation for `Constant`.
 */
template<typename Integer_, Integer_ value_>
struct is_Constant<Constant<Integer_, value_> > {
    /**
     * True, this class is a `Constant`.
     */
    static constexpr bool value = true;
};

/**
 * @cond
 */
template<typename Integer_, Integer_ value_>
struct is_Constant<const Constant<Integer_, value_> > {
    static constexpr bool value = true;
};
/**
 * @endcond
 */

}

#endif
