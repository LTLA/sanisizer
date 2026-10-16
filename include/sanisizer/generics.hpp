#ifndef SANISIZER_GENERICS_HPP
#define SANISIZER_GENERICS_HPP

#include <limits>
#include <type_traits>

#include "Bounded.hpp"
#include "Constant.hpp"

/**
 * @file generics.hpp
 * @brief Generics for operating on integer representations.
 */

namespace sanisizer {

/**
 * @tparam Value_ Integer type, `Bounded` or `Constant`.
 * @param x Integer value or an `Bounded` about an integer.
 * @return `x` if it is already an integer, otherwise `Bounded::value`.
 */
template<typename Value_>
constexpr auto get_value(const Value_ x) {
    if constexpr(std::is_integral<Value_>::value) {
        return x;
    } else if constexpr(is_Bounded<Value_>::value) {
        return x.value;
    } else {
        static_assert(is_Constant<Value_>::value);
        return Value_::value;
    }
}

/**
 * @tparam Value_ Integer type, `Bounded`, or `Constant`.
 * @return The maximum value of all instances of `Value_`.
 */
template<typename Value_>
constexpr auto get_max() {
    if constexpr(std::is_integral<Value_>::value) {
        return std::numeric_limits<Value_>::max();
    } else if constexpr(is_Bounded<Value_>::value) {
        return Value_::max;
    } else {
        static_assert(is_Constant<Value_>::value);
        return Value_::value;
    }
}

}

#endif
