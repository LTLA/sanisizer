#ifndef SANISIZER_ARITHMETIC_HPP
#define SANISIZER_ARITHMETIC_HPP

#include <limits>
#include <type_traits>

#include "attest.hpp"
#include "utils.hpp"

/**
 * @file arithmetic.hpp
 * @brief Safe arithmetic on integer sizes.
 */

namespace sanisizer {

/**
 * @cond
 */
template<typename Size_, typename Left_, typename Right_>
constexpr bool needs_sum_check() {
    constexpr Size_ smaxed = std::numeric_limits<Size_>::max();
    constexpr auto lmaxed = get_max<Left_>();
    if constexpr(as_unsigned(lmaxed) > as_unsigned(smaxed)) {
        return true;
    } else {
        constexpr Size_ delta = smaxed - static_cast<Size_>(lmaxed);
        constexpr auto rmaxed = get_max<Right_>();
        return as_unsigned(rmaxed) > as_unsigned(delta);
    }
}

template<typename Size_, typename Left_, typename Right_>
auto sum_protected(Left_ l, Right_ r) {
    check_negative(l);
    check_cast<Size_>(l);
    const Size_ lval = get_value(l);

    check_negative(r);
    check_cast<Size_>(r);
    const Size_ rval = get_value(r);

    if constexpr(needs_sum_check<Size_, Left_, Right_>()) {
        static_assert(std::is_integral<Size_>::value);
        constexpr Size_ smaxed = std::numeric_limits<Size_>::max();
        if (static_cast<Size_>(smaxed - lval) < rval) {
            throw std::overflow_error("overflow detected in sanisize::sum");
        }
        return Attestation<Size_, true, smaxed>(static_cast<Size_>(lval + rval));

    } else {
        constexpr Size_ maxsum = static_cast<Size_>(get_max<Left_>()) + static_cast<Size_>(get_max<Right_>());
        return Attestation<Size_, true, maxsum>(static_cast<Size_>(lval + rval));
    }
}

template<typename Size_, typename Left_, typename Right_, typename ... Args_>
auto sum_protected(Left_ left, Right_ right, Args_... more) {
    const auto subsum = sum_protected<Size_>(right, more...);
    return sum_protected<Size_>(left, subsum);
}

template<typename Size_, typename Left_, typename Right_>
Size_ sum_unsafe(Left_ left, Right_ right) {
    static_assert(std::is_integral<Size_>::value);
    static_assert(std::is_integral<Left_>::value);
    static_assert(std::is_integral<Right_>::value);
    return static_cast<Size_>(left) + static_cast<Size_>(right);
}
/**
 * @endcond
 */

/**
 * Add two or more values, checking for overflow in the output type.
 * This is typically used to compute the size of an array that is a concatenation of smaller arrays.
 *
 * @tparam Size_ Integer type representing some concept of size for an array/container.
 * @tparam First_ Integer type of the first value.
 * @tparam Second_ Integer type of the second value.
 * @tparam Args_ Integer types of additional values.
 *
 * @param first Non-negative value to add.
 * @param second Non-negative value to add.
 * @param more Additional non-negative values to add.
 *
 * @return Sum of all arguments as a `Size_`.
 * If overflow would occur, an `OverflowError` is raised.
 */
template<typename Size_, typename First_, typename ... Args_>
Size_ sum(First_ first, Args_... more) {
    return get_value(sum_protected<Size_>(first, more...));

}

/**
 * Unsafe version of `sum()` that casts its arguments to `Size_` but does not check for overflow.
 * This is more efficent if it is known that the sum will not overflow, e.g., from previous calls to `sum()` with larger values.
 *
 * @tparam Size_ Integer type representing some concept of size for an array/container.
 * @tparam First_ Integer type of the first value.
 * @tparam Second_ Integer type of the second value.
 * @tparam Args_ Integer types of additional values.
 * 
 * @param first Non-negative value to add.
 * @param second Non-negative value to add.
 * @param more Additional non-negative values to add.
 *
 * @return Sum of all arguments as a `Size_`.
 */
template<typename Size_, typename First_, typename Second_, typename ... Args_>
Size_ sum_unsafe(First_ first, Second_ second, Args_... more) {
    return sum_unsafe<Size_>(first, sum_unsafe<Size_>(second, more...));
}

/**
 * @cond
 */
template<typename Size_, typename Left_, typename Right_>
constexpr bool needs_product_check() {
    constexpr Size_ smaxed = std::numeric_limits<Size_>::max();
    constexpr auto lmaxed = get_max<Left_>();
    if constexpr(as_unsigned(lmaxed) > as_unsigned(smaxed)) {
        return true;
    } else if constexpr(lmaxed == 0) {
        return false;
    } else {
        constexpr auto ratio = smaxed / lmaxed;
        constexpr auto rmaxed = get_max<Right_>();
        return as_unsigned(rmaxed) > as_unsigned(ratio);
    }
}

template<typename Size_, typename Left_, typename Right_>
auto product_protected(Left_ l, Right_ r) {
    check_negative(l);
    check_cast<Size_>(l);
    const Size_ lval = get_value(l);

    check_negative(r);
    check_cast<Size_>(r);
    const Size_ rval = get_value(r);

    if constexpr(needs_product_check<Size_, Left_, Right_>()) {
        static_assert(std::is_integral<Size_>::value);
        constexpr Size_ smaxed = std::numeric_limits<Size_>::max();
        if (lval && static_cast<Size_>(smaxed / lval) < rval) {
            throw std::overflow_error("overflow detected in sanisize::product");
        }
        return Attestation<Size_, true, smaxed>(static_cast<Size_>(lval * rval));

    } else {
        constexpr Size_ maxprod = static_cast<Size_>(get_max<Left_>()) * static_cast<Size_>(get_max<Right_>());
        return Attestation<Size_, true, maxprod>(static_cast<Size_>(lval * rval));
    }
}

template<typename Size_, typename Left_, typename Right_, typename ... Args_>
auto product_protected(Left_ left, Right_ right, Args_... more) {
    auto subproduct = product_protected<Size_>(right, more...);
    return product_protected<Size_>(left, subproduct);
}

template<typename Size_, typename Left_, typename Right_>
Size_ product_unsafe(Left_ left, Right_ right) {
    static_assert(std::is_integral<Size_>::value);
    static_assert(std::is_integral<Left_>::value);
    static_assert(std::is_integral<Right_>::value);
    return static_cast<Size_>(left) * static_cast<Size_>(right);
}
/**
 * @endcond
 */

/**
 * Multiply two values, checking for overflow in the output type.
 * This is typically used to compute the size of a flattened N-dimensional array as the product of its dimension extents.
 * 
 * For consistency, this function will also check that each of `left` and `right` can be cast to `Size_`.
 * This ensures that per-dimension indices/extents can be safely represented as `Size_` in later steps (e.g., `nd_offset()`). 
 * These checls are necessary as the product may fit in `Size_` but not each of `left` and `right` if either is equal to zero.
 *
 * @tparam Size_ Integer type representing some concept of size for an array/container.
 * @tparam First_ Integer type of the first value.
 * @tparam Second_ Integer type of the second value.
 * @tparam Args_ Integer types of additional values.
 *
 * @param first Non-negative value to multiply.
 * @param second Non-negative value to multiply.
 * @param more Additional non-negative values to multiply.
 *
 * @return Product of all arguments as a `Size_`.
 * If overflow would occur, an `OverflowError` is raised.
 */
template<typename Size_, typename First_, typename ... Args_>
Size_ product(First_ first, Args_... more) {
    return get_value(product_protected<Size_>(first, more...));
}

/**
 * Unsafe version of `product()` that casts its arguments to `Size_` but does not check for overflow.
 * This is more efficent if it is known that the product will not overflow, e.g., from previous calls to `product()` with larger values.
 *
 * @tparam Size_ Integer type representing some concept of size for an array/container.
 * @tparam First_ Integer type of the first value.
 * @tparam Second_ Integer type of the second value.
 * @tparam Args_ Integer types of additional values.
 * 
 * @param first Non-negative value to multiply.
 * @param second Non-negative value to multiply.
 * @param more Additional non-negative values to multiply.
 *
 * @return Product of all arguments as a `Size_`.
 */
template<typename Size_, typename First_, typename Second_, typename ... Args_>
Size_ product_unsafe(First_ first, Second_ second, Args_... more) {
    return product_unsafe<Size_>(first, product_unsafe<Size_>(second, more...));
}

}

#endif
