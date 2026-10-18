#ifndef SANISIZER_CAST_HPP
#define SANISIZER_CAST_HPP

#include "utils.hpp"
#include "comparisons.hpp"
#include "generics.hpp"

/**
 * @file cast.hpp
 * @brief Safe casts of integer size.
 */

namespace sanisizer {

/**
 * @tparam Dest_ Integer type of the destination.
 * @tparam Value_ Integer type, `Bounded` or `Constant`.
 * @param x Representation of an integer value. 
 * @return Whether `x`'s value would overflow/wraparound when stored in `Dest_`.
 */
template<typename Dest_, typename Value_>
constexpr bool can_cast(Value_ x) {
    static_assert(std::is_integral<Dest_>::value);
    constexpr auto dest_maxed = get_max<Dest_>();
    if constexpr(is_less_than_or_equal(get_max<Value_>(), dest_maxed)) {
        return true;
    } else {
        return is_less_than_or_equal(get_value(x), dest_maxed);
    }
}

/**
 * @cond
 */
template<typename Dest_, typename Value_>
constexpr bool check_overflow(Value_ x) {
    if (!can_cast<Dest_>(x)) {
        throw std::overflow_error("detected " + failure<Dest_>() + " for integer cast");
    }
    return false;
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
    check_overflow<Dest_>(x);
    return get_value(x);
}

}

#endif
