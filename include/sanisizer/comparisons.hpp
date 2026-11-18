#ifndef SANISIZER_COMPARISONS_HPP
#define SANISIZER_COMPARISONS_HPP

#include <type_traits>

#include "utils.hpp"
#include "generics.hpp"

/**
 * @file comparisons.hpp
 * @brief Signedness-safe integer comparisons.
 */

namespace sanisizer {

/**
 * @cond
 */
// We convert all values to the appropriate unsigned integers to avoid warnings about comparing differently signed integers.
// In theory, this performs two unnecessary casts when comparing two signed integers.
// In practice, any decent compiler will just optimize this away by reinterpreting the bits of the signed integer.
// Then it's just a simple matter of zero-extending the smaller integer to the larger integer.
//
// To that end, we implement a little wrapper for conveniently casting to each integer's unsigned type (if it isn't already).
template<typename Integer_>
constexpr auto as_unsigned(Integer_ x) {
    static_assert(std::is_integral<Integer_>::value);
    if constexpr(std::is_unsigned<Integer_>::value) {
        return x;
    } else {
        return static_cast<typename std::make_unsigned<Integer_>::type>(x);
    }
}
/**
 * @endcond
 */

/**
 * @tparam Left_ Integer type of the value on the left hand side of the comparison.
 * This may also be a `Constant` or `Bounded`.
 * @tparam Right_ Integer type of the value on the right hand side of the comparison.
 * This may also be a `Constant` or `Bounded`.
 *
 * @param left Non-negative value on the left hand side of the comparison.
 * @param right Non-negative value on the right hand side of the comparison.
 *
 * @return Whether `left` is equal to `right`.
 */
template<typename Left_, typename Right_>
constexpr bool is_equal(Left_ left, Right_ right) {
    return as_unsigned(get_value(left)) == as_unsigned(get_value(right));
}

/**
 * @tparam Left_ Integer type of the value on the left hand side of the comparison.
 * This may also be a `Constant` or `Bounded`.
 * @tparam Right_ Integer type of the value on the right hand side of the comparison.
 * This may also be a `Constant` or `Bounded`.
 *
 * @param left Non-negative value on the left hand side of the comparison.
 * @param right Non-negative value on the right hand side of the comparison.
 *
 * @return Whether `left` is less than `right`.
 */
template<typename Left_, typename Right_>
constexpr bool is_less_than(Left_ left, Right_ right) {
    return as_unsigned(get_value(left)) < as_unsigned(get_value(right));
}

/**
 * @tparam Left_ Integer type of the value on the left hand side of the comparison.
 * This may also be a `Constant` or `Bounded`.
 * @tparam Right_ Integer type of the value on the right hand side of the comparison.
 * This may also be a `Constant` or `Bounded`.
 *
 * @param left Non-negative value on the left hand side of the comparison.
 * @param right Non-negative value on the right hand side of the comparison.
 *
 * @return Whether `left` is less than or equal to `right`.
 */
template<typename Left_, typename Right_>
constexpr bool is_less_than_or_equal(Left_ left, Right_ right) {
    return as_unsigned(get_value(left)) <= as_unsigned(get_value(right));
}

/**
 * @tparam Left_ Integer type of the value on the left hand side of the comparison.
 * This may also be a `Constant` or `Bounded`.
 * @tparam Right_ Integer type of the value on the right hand side of the comparison.
 * This may also be a `Constant` or `Bounded`.
 *
 * @param left Non-negative value on the left hand side of the comparison.
 * @param right Non-negative value on the right hand side of the comparison.
 *
 * @return Whether `left` is greater than `right`.
 */
template<typename Left_, typename Right_>
constexpr bool is_greater_than(Left_ left, Right_ right) {
    return as_unsigned(get_value(left)) > as_unsigned(get_value(right));
}

/**
 * @tparam Left_ Integer type of the value on the left hand side of the comparison.
 * This may also be a `Constant` or `Bounded`.
 * @tparam Right_ Integer type of the value on the right hand side of the comparison.
 * This may also be a `Constant` or `Bounded`.
 *
 * @param left Non-negative value on the left hand side of the comparison.
 * @param right Non-negative value on the right hand side of the comparison.
 *
 * @return Whether `left` is greater than or equal to `right`.
 */
template<typename Left_, typename Right_>
constexpr bool is_greater_than_or_equal(Left_ left, Right_ right) {
    return as_unsigned(get_value(left)) >= as_unsigned(get_value(right));
}

/**
 * @tparam First_ Integer type of the first value.
 * This may also be a `Bounded` or `Constant`.
 * @tparam Second_ Integer type of the second value.
 * This may also be a `Bounded` or `Constant`.
 *
 * @param first First non-negative value. 
 * @param second Second non-negative value. 
 *
 * @return The smaller of `first` and `second`, in the smaller integer type of `First_` and `Second_`.
 */
template<typename First_, typename Second_>
constexpr auto min(First_ first, Second_ second) {
    const auto fval = get_value(first);
    const auto sval = get_value(second);
    const bool first_larger = is_greater_than(fval, sval);

    // Deliberately using decltype(fval) instead of First_, and same with sval and Second_.
    // This ensures that we use the actual smaller integer type and ignore any Bounds.
    if constexpr(is_greater_than(get_max<I<decltype(fval)> >(), get_max<I<decltype(sval)> >())) {
        if (first_larger) {
            return sval;
        } else {
            return static_cast<I<decltype(sval)> >(fval);
        }
    } else {
        if (first_larger) {
            return static_cast<I<decltype(fval)> >(sval);
        } else {
            return fval;
        }
    }
}

/**
 * @tparam First_ Integer type of the first value.
 * This may also be a `Bounded` or `Constant`.
 * @tparam Second_ Integer type of the second value.
 * This may also be a `Bounded` or `Constant`.
 *
 * @param first First non-negative value. 
 * @param second Second non-negative value. 
 *
 * @return The larger of `first` and `second`, in the larger integer type of `First_` and `Second_`.
 */
template<typename First_, typename Second_>
constexpr auto max(First_ first, Second_ second) {
    const auto fval = get_value(first);
    const auto sval = get_value(second);
    const bool first_larger = is_greater_than(fval, sval);

    // Deliberately using decltype(fval) instead of First_, and same with sval and Second_.
    // This ensures that we use the actual larger integer type and ignore any Bounds.
    if constexpr(is_greater_than(get_max<I<decltype(fval)> >(), get_max<I<decltype(sval)> >())) {
        if (first_larger) {
            return fval;
        } else {
            return static_cast<I<decltype(fval)> >(sval);
        }
    } else {
        if (first_larger) {
            return static_cast<I<decltype(sval)> >(fval);
        } else {
            return sval;
        }
    }
}

}

#endif
