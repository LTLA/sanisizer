#ifndef SANISIZER_PRODUCT_HPP
#define SANISIZER_PRODUCT_HPP

#include <limits>
#include <type_traits>
#include <stdexcept>

#include "generics.hpp"
#include "overflow.hpp"
#include "Constant.hpp"
#include "utils.hpp"

/**
 * @file product.hpp
 * @brief Safe product of integer sizes.
 */

namespace sanisizer {

/**
 * @cond
 */
template<typename Dest_, typename First_, typename Second_>
constexpr bool needs_product_check() {
    constexpr Dest_ dest_maxed = std::numeric_limits<Dest_>::max();
    constexpr auto first_maxed = get_max<First_>();
    if constexpr(as_unsigned(first_maxed) > as_unsigned(dest_maxed)) {
        return true;
    } else if constexpr(first_maxed == 0) {
        return false;
    } else {
        constexpr auto ratio = dest_maxed / first_maxed;
        constexpr auto second_maxed = get_max<Second_>();
        return as_unsigned(second_maxed) > as_unsigned(ratio);
    }
}

template<typename Dest_, typename First_, typename Second_>
constexpr auto product_protected(First_ first, Second_ second) {
    if (does_overflow<Dest_>(first)) {
        throw std::overflow_error("detected " + failure<Dest_>() + " of multiplication term");
    }
    const Dest_ first_val = get_value(first);

    if (does_overflow<Dest_>(second)) {
        throw std::overflow_error("detected " + failure<Dest_>() + " of multiplication term");
    }
    const Dest_ second_val = get_value(second);

    if constexpr(needs_product_check<Dest_, First_, Second_>()) {
        static_assert(std::is_integral<Dest_>::value);
        constexpr Dest_ dest_maxed = std::numeric_limits<Dest_>::max();

        const bool overflowed =[&]{
            if constexpr(is_Constant<Second_>::value) {
                // If Second_ is Constant, then the compiler could compute 'dest_maxed / second_val' at compile time, which saves us some runtime work.
                return (second_val && static_cast<Dest_>(dest_maxed / second_val) < first_val);
            } else {
                return (first_val && static_cast<Dest_>(dest_maxed / first_val) < second_val);
            }
        }();
        if (overflowed) {
            throw std::overflow_error("detected " + failure<Dest_>() + " during multiplication");
        }

        return static_cast<Dest_>(first_val * second_val);

    } else {
        constexpr Dest_ maxprod = static_cast<Dest_>(get_max<First_>()) * static_cast<Dest_>(get_max<Second_>());
        return set_bound<Dest_, maxprod>(static_cast<Dest_>(first_val * second_val));
    }
}

template<typename Dest_, typename First_, typename Second_, typename ... Args_>
constexpr auto product_protected(First_ first, Second_ second, Args_... more) {
    const auto subproduct = product_protected<Dest_>(first, second);
    return product_protected<Dest_>(subproduct, more...);
}
/**
 * @endcond
 */

/**
 * Multiply two or more non-negative values, checking for overflow/wraparound in the destination type.
 * This is typically used to compute the size of a flattened N-dimensional array as the product of its dimension extents.
 * 
 * For consistency, this function will also check that each input value can be cast to `Dest_`.
 * This ensures that per-dimension indices/extents can be safely represented as `Dest_` in later steps (e.g., `nd_offset()`). 
 * These checks are necessary as the product may fit in `Dest_` but not the input values if one of the inputs is zero. 
 *
 * @tparam Dest_ Integer type of the destination.
 * @tparam First_ Integer type of the first value.
 * This may be also be a `Bounded` or `Constant`.
 * @tparam Args_ Integer types of additional values.
 * Any number of these may also be `Bounded` or `Constant`.
 *
 * @param first Non-negative value to multiply.
 * @param more Additional non-negative values to multiply.
 *
 * @return Product of all arguments as a `Dest_`.
 *
 * @throw std::overflow_error Thrown if an overflow/wraparound would occur.
 */
template<typename Dest_, typename First_, typename ... Args_>
constexpr Dest_ product(First_ first, Args_... more) {
    return get_value(product_protected<Dest_>(first, more...));
}

/**
 * @cond
 */
template<typename Dest_, typename First_, typename Second_>
constexpr Dest_ product_unprotected(First_ left, Second_ right) {
    static_assert(std::is_integral<Dest_>::value);
    return static_cast<Dest_>(get_value(left)) * static_cast<Dest_>(get_value(right));
}

template<typename Dest_, typename First_, typename Second_, typename ... Args_>
constexpr Dest_ product_unprotected(First_ left, Second_ right, Args_... more) {
    const auto subproduct = product_unprotected<Dest_>(left, right);
    return product_unprotected<Dest_>(subproduct, more...);
}
/**
 * @endcond
 */

/**
 * Unsafe alternative to `product()` that casts its arguments to `Dest_` but does not check for overflow/wraparound.
 * This is more efficent than `product()` if it is already known that the product will not overflow,
 * e.g., when computing `std::size_t` offsets into a pre-allocated array.
 *
 * @tparam Dest_ Integer type of the destination.
 * @tparam First_ Type of the first multiplication term.
 * This should be an integer type, `Bounded` or `Constant`.
 * @tparam Args_ Types of additional terms.
 * Each of these should be either an integer type, `Bounded` or `Constant`.
 * 
 * @param first Non-negative term to multiply.
 * @param more Additional non-negative terms to multiply.
 *
 * @return Product of all arguments as a `Dest_`.
 */
template<typename Dest_, typename First_, typename ... Args_>
constexpr Dest_ product_unsafe(First_ first, Args_... more) {
    return product_unprotected<Dest_>(first, more...);
}

}

#endif
