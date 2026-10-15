#ifndef SANISIZER_SUM_HPP
#define SANISIZER_SUM_HPP

#include <limits>
#include <type_traits>
#include <stdexcept>

#include "generics.hpp"
#include "Constant.hpp"
#include "utils.hpp"

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
    constexpr Dest_ dest_maxed = std::numeric_limits<Dest_>::max();
    constexpr auto first_maxed = get_max<First_>();
    if constexpr(as_unsigned(first_maxed) > as_unsigned(dest_maxed)) {
        return true;
    } else {
        constexpr Dest_ delta = dest_maxed - static_cast<Dest_>(first_maxed);
        constexpr auto second_maxed = get_max<Second_>();
        return as_unsigned(second_maxed) > as_unsigned(delta);
    }
}

template<typename Dest_, typename First_, typename Second_>
constexpr auto sum_protected(First_ first, Second_ second) {
    check_overflow<Dest_>(first);
    const Dest_ first_val = get_value(first);

    check_overflow<Dest_>(second);
    const Dest_ second_val = get_value(second);

    if constexpr(needs_sum_check<Dest_, First_, Second_>()) {
        static_assert(std::is_integral<Dest_>::value);
        constexpr Dest_ dest_maxed = std::numeric_limits<Dest_>::max();

        const bool overflowed =[&]{
            if constexpr(is_Constant<Second_>::value) {
                // If Second_ is Constant, then the compiler could compute 'dest_maxed - second_val' at compile time, which saves us some runtime work.
                return (static_cast<Dest_>(dest_maxed - second_val) < first_val);
            } else {
                return (static_cast<Dest_>(dest_maxed - first_val) < second_val);
            }
        }();
        if (overflowed) {
            throw std::overflow_error("overflow detected during summation");
        }

        return static_cast<Dest_>(first_val + second_val);

    } else {
        constexpr Dest_ maxsum = static_cast<Dest_>(get_max<First_>()) + static_cast<Dest_>(get_max<Second_>());
        return set_bound<Dest_, maxsum>(static_cast<Dest_>(first_val + second_val));
    }
}

template<typename Dest_, typename First_, typename Second_, typename ... Args_>
constexpr auto sum_protected(First_ first, Second_ second, Args_... more) {
    const auto subsum = sum_protected<Dest_>(first, second); 
    return sum_protected<Dest_>(subsum, more...);
}

template<typename Dest_, typename First_, typename Second_>
constexpr Dest_ sum_unprotected(First_ first, Second_ second) {
    static_assert(std::is_integral<Dest_>::value);
    static_assert(std::is_integral<First_>::value);
    static_assert(std::is_integral<Second_>::value);
    return static_cast<Dest_>(first) + static_cast<Dest_>(second);
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
 * Add two or more non-negative values, checking for overflow in the destination type.
 * This is typically used to compute the size of an array that is a concatenation of smaller arrays.
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
 * @throw std::overflow_error Thrown if an overflow would occur.
 */
template<typename Dest_, typename First_, typename ... Args_>
constexpr Dest_ sum(First_ first, Args_... more) {
    return get_value(sum_protected<Dest_>(first, more...));
}

/**
 * Unsafe version of `sum()` that casts its arguments to `Dest_` before summation but does not check for overflow.
 * This is more efficent if it is already known that the sum will not overflow, e.g., from previous calls to `sum()` with larger values.
 *
 * @tparam Dest_ Integer type of the destination.
 * @tparam First_ Integer type of the first value.
 * @tparam Args_ Integer types of additional values.
 * 
 * @param first Non-negative value to add.
 * @param more Additional non-negative values to add.
 *
 * @return Sum of all arguments as a `Dest_`.
 */
template<typename Dest_, typename First_, typename ... Args_>
constexpr Dest_ sum_unsafe(First_ first, Args_... more) {
    return sum_unprotected<Dest_>(first, more...);
}

}

#endif
