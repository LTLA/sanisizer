#ifndef SANISIZER_OVERFLOW_HPP
#define SANISIZER_OVERFLOW_HPP

#include <limits>
#include <type_traits>

#include "generics.hpp"

/**
 * @file overflow.hpp
 * @brief Check for integer overflow/wraparound.
 */

namespace sanisizer {

/**
 * @tparam Dest_ Integer type of the destination.
 * @tparam Value_ Integer type, `Bounded` or `Constant`.
 * @param x Representation of an integer value. 
 * @return Whether `x`'s value would overflow/wraparound when stored in `Dest_`.
 */
template<typename Dest_, typename Value_>
constexpr bool does_overflow(Value_ x) {
    static_assert(std::is_integral<Dest_>::value);
    constexpr auto umaxed = as_unsigned(std::numeric_limits<Dest_>::max());
    if constexpr(umaxed < as_unsigned(get_max<Value_>())) {
        return umaxed < as_unsigned(get_value(x));
    }
    return false;
}

/**
 * @cond
 */
template<typename Dest_, typename Value_>
constexpr bool check_overflow(Value_ x) {
    if (does_overflow<Dest_>(x)) {
        throw std::overflow_error("detected " + failure<Dest_>() + " for integer cast");
    }
    return false;
}
/**
 * @endcond
 */

}

#endif
