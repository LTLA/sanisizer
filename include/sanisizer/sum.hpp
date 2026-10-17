#ifndef SANISIZER_SUM_HPP
#define SANISIZER_SUM_HPP

#include <type_traits>
#include <stdexcept>
#include <optional>

#include "generics.hpp"
#include "Bounded.hpp"
#include "Constant.hpp"
#include "utils.hpp"
#include "comparisons.hpp"

/**
 * @file sum.hpp
 * @brief Safe sum of integer sizes.
 */

namespace sanisizer {

/**
 * @cond
 */
template<typename Dest_, typename First_, typename Second_>
constexpr bool needs_sum_check() {
    constexpr Dest_ dest_maxed = get_max<Dest_>();
    constexpr auto first_maxed = get_max<First_>();
    if constexpr(is_greater_than(first_maxed, dest_maxed)) {
        return true;
    } else {
        constexpr Dest_ delta = dest_maxed - static_cast<Dest_>(first_maxed);
        return is_greater_than(get_max<Second_>(), delta);
    }
}

template<typename Dest_, typename First_, typename Second_>
constexpr bool does_sum_overflow_ordered(First_ first, Second_ second) {
    constexpr Dest_ dest_maxed = get_max<Dest_>();
    const auto first_val = get_value(first);
    if (is_greater_than(first_val, dest_maxed)) { // This check can be compiled out if First_ is not greater than Dest_.
        return true;
    } else {
        const Dest_ delta = dest_maxed - static_cast<Dest_>(first_val); // If First_ is Constant, then the compiler could compute 'delta' at compile time.
        return is_greater_than(get_value(second), delta);
    }
}

template<typename Dest_, typename First_, typename Second_>
constexpr bool does_sum_overflow(First_ first, Second_ second) {
    if constexpr(is_Constant<First_>::value) {
        // Shift threshold calculation to compile-time.
        return does_sum_overflow_ordered<Dest_>(first, second);

    } else if constexpr(is_Constant<Second_>::value) {
        // Shift threshold calculation to compile-time.
        return does_sum_overflow_ordered<Dest_>(second, first);

    } else if constexpr(is_less_than_or_equal(get_max<First_>(), get_max<Dest_>())) {
        // Skip overflow check when First_ is not greater than Dest_.
        return does_sum_overflow_ordered<Dest_>(first, second);

    } else {
        // Skip overflow check if Second_ is not greater than Dest_.
        // If not, the order doesn't matter w.r.t. the amount of work, so we might as well use this as the default case.
        return does_sum_overflow_ordered<Dest_>(second, first);
    }
}

template<typename Dest_, typename First_, typename Second_>
constexpr auto sum_protected(First_ first, Second_ second) {
    static_assert(std::is_integral<Dest_>::value);

    if constexpr(needs_sum_check<Dest_, First_, Second_>()) {
        if (does_sum_overflow<Dest_>(first, second)) {
            return std::optional<Dest_>();
        } else {
            // Cast before sum to avoid overflow in the input types or whatever integer promotion decides to use.
            return std::make_optional<Dest_>(static_cast<Dest_>(get_value(first)) + static_cast<Dest_>(get_value(second)));
        }

    } else {
        constexpr Dest_ maxsum = static_cast<Dest_>(get_max<First_>()) + static_cast<Dest_>(get_max<Second_>());
        if constexpr(is_Constant<First_>::value && is_Constant<Second_>::value) {
            // Return a Constant in case we can do more compile-time stuff in the next chained operation.
            // No need to this when 'needs_sum_check' is true as it will always fail with two Constants.
            return std::make_optional(Constant<Dest_, maxsum>());
        } else {
            return std::make_optional<Bounded<Dest_, maxsum> >(static_cast<Dest_>(get_value(first)) + static_cast<Dest_>(get_value(second)));
        }
    }
}

template<typename Dest_, typename First_, typename Second_, typename ... Args_>
constexpr auto sum_protected(First_ first, Second_ second, Args_... more) {
    const auto subsum = sum_protected<Dest_>(first, second); 
    if (subsum.has_value()) {
        return sum_protected<Dest_>(*subsum, more...);
    } else {
        return I<decltype(sum_protected<Dest_>(*subsum, more...))>();
    }
}

/**
 * @endcond
 */

/**
 * Add two or more non-negative values.
 * If overflow/wraparound in the destination type is detected, an empty object is returned.
 * (See also `sum()` to throw an exception upon overflow/wraparound.)
 *
 * @tparam Dest_ Integer type of the destination.
 * @tparam First_ Integer type of the first value.
 * This may also be a `Bounded` or `Constant`.
 * @tparam Args_ Integer types of additional values.
 * Any number of these may also be `Bounded` or `Constant`.
 *
 * @param first First non-negative value to add.
 * @param more Additional non-negative values to add.
 *
 * @return Sum of all arguments as a `Dest_`, wrapped in a `std::optional`.
 * An empty object is returned if overflow/wraparound is detected in the destination type at any step of the summation.
 */
template<typename Dest_, typename First_, typename ... Args_>
constexpr std::optional<Dest_> sum_maybe(First_ first, Args_... more) {
    const auto res = sum_protected<Dest_>(first, more...);
    if (res.has_value()) {
        return std::make_optional<Dest_>(get_value(*res));
    } else {
        return std::optional<Dest_>();
    }
}

/**
 * Add two or more non-negative values, e.g., to compute the size of an array that is a concatenation of smaller arrays.
 * If overflow/wraparound in the destination type is detected, an exception is thrown.
 * (See also `sum_maybe()` to return an empty object upon overflow/wraparound.)
 *
 * @tparam Dest_ Integer type of the destination.
 * @tparam First_ Integer type of the first value.
 * This may also be a `Bounded` or `Constant`.
 * @tparam Args_ Integer types of additional values.
 * Any number of these may also be `Bounded` or `Constant`.
 *
 * @param first First non-negative value to add.
 * @param more Additional non-negative values to add.
 *
 * @return Sum of all arguments as a `Dest_`.
 *
 * @throw std::overflow_error Thrown if an overflow/wraparound occurs at any summation step.
 */
template<typename Dest_, typename First_, typename ... Args_>
constexpr Dest_ sum(First_ first, Args_... more) {
    const auto res = sum_maybe<Dest_>(first, more...);
    if (!res.has_value()) {
        throw std::overflow_error("detected " + failure<Dest_>() + " during summation");
    }
    return *res;
}

/**
 * Convenience wrapper for `sum<Dest_>(x, 1)`, i.e., `get_value(x) + 1` with an exception if overflow/wraparound is detected.
 *
 * @tparam Dest_ Integer type of the destination.
 * @tparam Value_ Integer type of the value.
 * This may also be a `Bounded` or `Constant`.
 *
 * @param x Non-negative value.
 * 
 * @return The value of `x` plus 1.
 */
template<typename Dest_, typename Value_>
Dest_ plus_one(Value_ x) {
    return sum<Dest_>(x, Constant<int, 1>());
}

/**
 * Convenience wrapper for `x = plus_one<Dest_>(x)`, i.e., `++x` with an exception if overflow/wraparound is detected.
 *
 * @tparam Dest_ Integer type of the destination.
 * @tparam Value_ Integer type of the value.
 * 
 * @param x Non-negative value.
 * On output, this is incremented by 1.
 *
 * @return Reference to `x` after increment.
 */
template<typename Integer_>
Integer_& increment(Integer_& x) {
    x = plus_one<Integer_>(x);
    return x;
}

/**
 * @cond
 */
template<typename Dest_, typename First_, typename Second_>
constexpr Dest_ sum_unprotected(First_ first, Second_ second) {
    static_assert(std::is_integral<Dest_>::value);
    return static_cast<Dest_>(get_value(first)) + static_cast<Dest_>(get_value(second));
}

template<typename Dest_, typename First_, typename Second_, typename ... Args_>
constexpr Dest_ sum_unprotected(First_ first, Second_ second, Args_... more) {
    const auto subsum = sum_unprotected<Dest_>(first, second);
    return sum_unprotected<Dest_>(subsum, more...);
}
/**
 * @endcond
 */

/**
 * Unsafe alternative to `sum()` that casts its arguments to `Dest_` before summation but does not check for overflow/wraparound.
 * This is more efficent than `sum()` if it is already known that the sum will not overflow, 
 * e.g., when computing `std::size_t` offsets into a pre-allocated array.
 *
 * @tparam Dest_ Integer type of the destination.
 * @tparam First_ Type of the first summation term.
 * This should be an integer type, `Bounded` or `Constant`.
 * @tparam Args_ Types of additional terms.
 * Each of these should be either an integer type, `Bounded` or `Constant`.
 * 
 * @param first Non-negative term to sum.
 * @param more Additional non-negative terms to add.
 *
 * @return Sum of all arguments as a `Dest_`.
 */
template<typename Dest_, typename First_, typename ... Args_>
constexpr Dest_ sum_unsafe(First_ first, Args_... more) {
    return sum_unprotected<Dest_>(first, more...);
}

}

#endif
