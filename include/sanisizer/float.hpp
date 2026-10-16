#ifndef SANISIZER_FLOAT_HPP
#define SANISIZER_FLOAT_HPP

#include <limits>
#include <cmath>
#include <stdexcept>
#include <type_traits>
#include <cassert>

#include "utils.hpp"
#include "generics.hpp"

/**
 * @file float.hpp
 * @brief Cast floats to/from integer sizes.
 */

namespace sanisizer {

/**
 * @tparam Float_ Floating-point type.
 *
 * @param x A finite non-negative integer.
 * Specifically, a floating-point number for which the following is true: `std::isfinite(x)`, `x >= 0` and `x == std::trunc(x)`.
 *
 * @return Number of bits required to store `x` exactly in a (two's complement) integer type.
 *
 * The return value can be compared to `std::numeric_limits<Integer_>::digits` to determine if `Integer_` is large enough to store `x`.
 */
template<typename Float_>
int required_bits_for_float(Float_ x) {
    static_assert(std::is_floating_point<Float_>::value);
    assert(x >= 0);
    assert(std::isfinite(x));
    assert(std::trunc(x) == x);

#ifndef SANISIZER_FLOAT_FORCE_FREXP
    if constexpr(std::numeric_limits<Float_>::radix == 2) {
        if (x == 0) {
            return 0;
        } else {
            // ilogb returns an 'exp' such that 2^exp <= x < 2^(exp + 1).
            return std::ilogb(x) + 1;
        }
    } else {
#endif
        // Ensure we're covered for weird float types where the radix is not 2.
        int exp;
        std::frexp(x, &exp); 
        // frexp guarantees that 2^(exp - 1) <= x < 2^exp.
        return exp;
#ifndef SANISIZER_FLOAT_FORCE_FREXP
    }
#endif
}

/**
 * Cast a non-negative floating-point number to an integer.
 * This checks that the conversion is possible, otherwise it throws an exception. 
 * Such a cast is occasionally necessary when floating-point calculations are used to derive the size of a container, number of loop iterations, etc.
 *
 * @tparam Integer_ Integer type.
 * @tparam Float_ Floating-point type.
 *
 * @param x Floating-point number, usually holding some kind of size. 
 *
 * @return The value of `x` as an integer, after truncation.
 *
 * @throw std::domain_error Thrown if `x` is negative or non-finite.
 * @throw std::overflow_error Thrown if `x` is too large to be stored in `Integer_`.
 */
template<typename Integer_, typename Float_>
Integer_ from_float(Float_ x) {
    static_assert(std::is_floating_point<Float_>::value);
    static_assert(std::is_integral<Integer_>::value);

    if (!std::isfinite(x)) {
        throw std::range_error("expected a finite value");
    }
    if (x < 0) {
        throw std::range_error("expected a non-negative value");
    }
    x = std::trunc(x);

    constexpr auto output_precision = std::numeric_limits<Integer_>::digits;
    if (required_bits_for_float(x) > output_precision) {
        throw std::overflow_error("value is too large for the specified integer type");
    }

    return x;
}

/**
 * Cast a non-negative integer into a (IEE-754 compliant) floating-point number.
 * This checks that (i) there is no loss of precision and (ii) all integers smaller than the input can also be represented by the specified floating-point type.
 *
 * This is motivated by the fact that some environments do not have a dedicated integer type (e.g., Javascript). 
 * In such cases, array lengths need to be stored as floating-point values.
 * Our checks ensure that both the array length and all valid array indices can be exactly represented in the floating-point type.
 *
 * Note that the purpose of this function is to preserve precision rather than to avoid undefined behavior.
 * For IEEE-754 floats, very large integers already have a well-defined cast to positive infinity. 
 *
 * @tparam Float_ Floating-point type.
 * This should be one of the IEEE-754 binary formats.
 * @tparam Value_ Integer type, `Bounded` or `Constant`.
 *
 * @param x Non-negative integer, usually holding some kind of size. 
 *
 * @return The value of `x` as a floating-point number.
 *
 * @throw std::overflow_error Thrown if `Float_` cannot exactly represent all non-negative integers less than or equal to `x`.
 */
template<typename Float_, typename Value_>
Float_ to_float(Value_ x) {
    const auto val = get_value(x);

    // protect against the various -1 operations.
    constexpr auto xmax = get_max<I<decltype(x)> >();
    if constexpr(xmax == 0) {
        return 0;
    } else if (val == 0) {
        return 0;
    }

    constexpr auto frad = std::numeric_limits<Float_>::radix;
    constexpr auto fdig = std::numeric_limits<Float_>::digits;

    // Only considering IEEE-754 binary formats, to keep things simple.
    // Also I don't even have the hardware to test the other formats.
    static_assert(frad == 2);
    static_assert(std::numeric_limits<Float_>::is_iec559);

    // Checking digits first, because attempting to right-shift an integer by a number equal to or more than its bits is UB.
    if constexpr(std::numeric_limits<I<decltype(val)> >::digits > fdig) {
        // We can represent '2^fdig' itself due to the implicit leading bit on the mantissa in a (normal) IEEE binary float.
        // Thus, we subtract 1 to avoid throwing an error when 'x == 2^fdig'.
        if constexpr((xmax - 1) >> fdig) {
            const auto y = (val - 1) >> fdig;
            if (y) {
                throw std::overflow_error("value is too large for the specified floating-point type");
            }
        }
    }

    return val;
}

}

#endif
