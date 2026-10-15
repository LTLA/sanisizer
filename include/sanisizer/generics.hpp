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

/**
 * @tparam Dest_ Integer type of the destination.
 * @tparam Value_ Integer type, `Bounded` or `Constant`.
 * @param x Representation of an integer value. 
 * @return An overflow error is thrown if `x`'s value would overflow when stored in `Dest_`.
 * Otherwise, `false` is returned.
 */
template<typename Dest_, typename Value_>
constexpr bool check_overflow(Value_ x) {
    static_assert(std::is_integral<Dest_>::value);
    constexpr auto umaxed = as_unsigned(std::numeric_limits<Dest_>::max());
    if constexpr(umaxed < as_unsigned(get_max<Value_>())) {
        if (umaxed < as_unsigned(get_value(x))) {
            throw std::overflow_error("overflow detected for integer cast");
        }
    }
    return false;
}

/**
 * @tparam Max_ Integer type of the new compile-time maximum.
 * @tparam new_max_ The new compile-time maximum.
 * @tparam Value_ Integer type, `Bounded` or `Constant`.
 * @param x Representation of an integer value. 
 * @return `x` if it is already known to be less than or equal to `new_max_`, 
 * otherwise, an `Bounded` that declares this constraint.
 *
 * @throw std::domain_error Thrown if the value of `x` is greater than `new_max_`.
 */
template<typename Max_, Max_ new_max_, typename Value_>
constexpr auto set_bound(Value_ x) {
    if constexpr(as_unsigned(get_max<Value_>()) <= as_unsigned(new_max_)) {
        return x; 
    } else {
        const auto val = get_value(x);
        if (as_unsigned(val) > as_unsigned(new_max_)) {
            throw std::domain_error("input value exceeds the declared upper bound");
        }
        return Bounded<I<decltype(val)>, static_cast<I<decltype(val)> >(new_max_)>(val);
    }
}

/**
 * Variant of `set_bound()` that uses the maximum value of an integer type.
 *
 * @tparam MaxType_ Integer type of the new compile-time maximum.
 * @tparam Value_ Integer type, `Bounded` or `Constant`.
 * @param x Representation of an integer value. 
 * @return `x` if it is already known to be less than or equal to the maximum value of `MaxType_`;
 * otherwise, an `Bounded` that declares this constraint.
 */
template<typename MaxType_, typename Value_>
constexpr auto set_bound_by_type(Value_ x) {
    return set_bound<MaxType_, std::numeric_limits<MaxType_>::max()>(x);
}

/** 
 * @cond
 */
// Back-compatibility.
template<typename Max_, Max_ new_max_, typename Value_>
constexpr auto attest_max(Value_ x) { return set_bound<Max_, new_max_, Value_>(x); }

template<typename MaxType_, typename Value_>
constexpr auto attest_max_by_type(Value_ x) { return set_bound_by_type<MaxType_, Value_>(x); }
/** 
 * @endcond
 */

}

#endif
