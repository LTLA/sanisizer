#ifndef SANISIZER_CAST_HPP
#define SANISIZER_CAST_HPP

#include "utils.hpp"
#include "overflow.hpp"

/**
 * @file cast.hpp
 * @brief Safe casts of integer size.
 */

namespace sanisizer {

/**
 * Check if a non-negative integer can be cast to a destination type, typically the size type of a C-style array or STL container. 
 * This is useful for chaining together checks without actually doing the cast itself.
 * 
 * @tparam Dest_ Integer type of the destination.
 * @tparam Value_ Integer type, `Bounded` or `Constant`.
 *
 * @param x Non-negative value to be casted.
 *
 * @return `x`, if the maximum value of `x` is less than or equal to that of `Dest_`.
 * Otherwise, a `Bounded` object is returned that contains `x` and sets the maximum value to that of `Dest` (see `set_bound_by_type()`).
 *
 * @throw std::overflow_error Thrown if overflow/wraparound would occur.
 */
template<typename Dest_, typename Value_>
constexpr auto pre_cast(Value_ x) {
    if constexpr(get_max<Value_>() <= get_max<Dest_>()) {
        return x;
    } else {
        if (does_overflow<Dest_>(x)) {
            throw std::overflow_error("detected " + failure<Dest_>() + " during integer cast");
        }
        return set_bound_by_type<Dest_>(get_value(x));
    }
}

/**
 * @cond
 */
// Back-compatibility.
template<typename Dest_, typename Value_>
constexpr auto can_cast(Value_ x) {
    return get_value(pre_cast<Dest_>(x));
}
/**
 * @endcond
 */

/**
 * Cast a non-negative integer to a destination type, typically the size type of a C-style array or STL container.
 * This avoids accidental overflow/wraparound from an implicit cast when `x` is used in `new` or the container's constructor.
 * 
 * @tparam Dest_ Integer type of the destination.
 * @tparam Value_ Integer type, `Bounded` or `Constant`.
 *
 * @param x Non-negative value to be casted.
 *
 * @return `x` as a `Dest_`.
 * An error is thrown if overflow/wraparound would occur.
 */
template<typename Dest_, typename Value_>
constexpr Dest_ cast(Value_ x) {
    return can_cast<Dest_>(x);
}

}

#endif
