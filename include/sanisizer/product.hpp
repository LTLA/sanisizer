#ifndef SANISIZER_PRODUCT_HPP
#define SANISIZER_PRODUCT_HPP

#include <type_traits>
#include <stdexcept>
#include <optional>

#include "generics.hpp"
#include "Bounded.hpp"
#include "Constant.hpp"
#include "utils.hpp"
#include "comparisons.hpp"

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
    constexpr Dest_ dest_maxed = get_max<Dest_>();
    constexpr auto first_maxed = get_max<First_>();
    if constexpr(is_greater_than(first_maxed, dest_maxed)) {
        return true;
    } else if constexpr(first_maxed == 0) {
        // We are obliged to check that all arguments fit into Dest_, but this is only necessary if first_maxed == 0.
        return is_greater_than(get_max<Second_>(), dest_maxed);
    } else {
        constexpr auto ratio = dest_maxed / static_cast<Dest_>(first_maxed);
        return is_greater_than(get_max<Second_>(), ratio);
    }
}

template<typename Dest_, typename First_, typename Second_>
constexpr bool does_product_overflow_ordered(First_ first, Second_ second) {
    constexpr Dest_ dest_maxed = get_max<Dest_>();
    const auto first_val = get_value(first);
    if (is_greater_than(first_val, dest_maxed)) { // This check can be compiled out if First_ is not greater than Dest_.
        return true;
    } else if (first_val == 0) {
        // We are obliged to check that all arguments fit into Dest_, but this is only necessary if first_val == 0.
        return is_greater_than(get_value(second), dest_maxed);
    } else {
        const auto ratio = dest_maxed / static_cast<Dest_>(first_val); // If First_ is Constant, then the compiler could compute 'ratio' at compile time.
        return is_greater_than(get_value(second), ratio);
    }
}

template<typename Dest_, typename First_, typename Second_>
constexpr bool does_product_overflow(First_ first, Second_ second) {
    if constexpr(is_Constant<First_>::value) {
        // Shift threshold calculation to compile-time.
        return does_product_overflow_ordered<Dest_>(first, second);

    } else if constexpr(is_Constant<Second_>::value) {
        // Shift threshold calculation to compile-time.
        return does_product_overflow_ordered<Dest_>(second, first);

    } else if constexpr(is_less_than_or_equal(get_max<First_>(), get_max<Dest_>())) {
        // Skip overflow check when First_ is not greater than Dest_.
        return does_product_overflow_ordered<Dest_>(first, second);

    } else {
        // Skip overflow check if Second_ is not greater than Dest_.
        // If not, the order doesn't matter w.r.t. the amount of work, so we might as well use this as the default case.
        return does_product_overflow_ordered<Dest_>(second, first);
    }
}

template<typename Dest_, typename First_, typename Second_>
constexpr auto product_protected(First_ first, Second_ second) {
    static_assert(std::is_integral<Dest_>::value);

    if constexpr(needs_product_check<Dest_, First_, Second_>()) {
        if (does_product_overflow<Dest_>(first, second)) {
            return std::optional<Dest_>();
        } else {
            // Cast before product to avoid overflow in the input types or whatever integer promotion decides to use.
            return std::make_optional<Dest_>(static_cast<Dest_>(get_value(first)) * static_cast<Dest_>(get_value(second)));
        }

    } else {
        constexpr Dest_ maxprod = static_cast<Dest_>(get_max<First_>()) * static_cast<Dest_>(get_max<Second_>());
        if constexpr(is_Constant<First_>::value && is_Constant<Second_>::value) {
            // Return a Constant in case we can do more compile-time stuff in the next chained operation.
            // No need to this when 'needs_product_check' is true as it will always fail with two Constants.
            return std::make_optional(Constant<Dest_, maxprod>());
        } else {
            return std::make_optional<Bounded<Dest_, maxprod> >(static_cast<Dest_>(get_value(first)) * static_cast<Dest_>(get_value(second)));
        }
    }
}

template<typename Dest_, typename First_, typename Second_, typename ... Args_>
constexpr auto product_protected(First_ first, Second_ second, Args_... more) {
    const auto subproduct = product_protected<Dest_>(first, second);
    if (subproduct.has_value()) {
        return product_protected<Dest_>(*subproduct, more...);
    } else {
        return I<decltype(product_protected<Dest_>(*subproduct, more...))>();
    }
}
/**
 * @endcond
 */

/**
 * Multiply two or more non-negative values.
 * If overflow/wraparound in the destination type is detected, an empty object is returned.
 * See also `product()` to throw an exception upon overflow/wraparound.
 * 
 * For consistency, this function will also check that each input value can be cast to `Dest_`.
 * See `product()` for more details.
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
constexpr std::optional<Dest_> product_maybe(First_ first, Args_... more) {
    const auto res = product_protected<Dest_>(first, more...);
    if (res.has_value()) {
        return std::make_optional<Dest_>(get_value(*res));
    } else {
        return std::optional<Dest_>();
    }
}

/**
 * Multiply two or more non-negative values, e.g., to compute the size of a flattened N-dimensional array as the product of its dimension extents.
 * If overflow/wraparound in the destination type is detected, an exception is thrown.
 * (See also `product_maybe()` to return an empty object upon overflow/wraparound.)
 * 
 * For consistency, this function will also check that each input value can be cast to `Dest_`.
 * This ensures that per-dimension indices/extents can be safely represented as `Dest_` in later steps (e.g., `nd_offset()`). 
 * These checks are necessary as the product may fit in `Dest_` but not the input values if any one of the inputs is zero. 
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
    const auto res = product_maybe<Dest_>(first, more...);
    if (!res.has_value()) {
        throw std::overflow_error("detected " + failure<Dest_>() + " during multiplication");
    }
    return *res;
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
