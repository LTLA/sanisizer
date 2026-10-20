#ifndef SANISIZER_BOUNDED_HPP
#define SANISIZER_BOUNDED_HPP

#include <type_traits>
#include <cassert>
#include <stdexcept>

#include "utils.hpp"

/**
 * @file Bounded.hpp
 * @brief Specify compile-time bounds on integers.
 */

namespace sanisizer {

/**
 * @brief Bound the maximum possible value of an integer.
 *
 * @tparam Integer_ Type of the integer.
 * @tparam max_ Maximum value of the integer, known at compile time.
 * This should be non-negative.
 *
 * Specifies that an integer has a maximum upper bound of `max_`, known at compile-time.
 * The upper bound may be less than the maxium value of `Integer_`, which enables some optimizations in functions like `sum()` and `product()`.
 * For example, we could attest that a `std::size_t` has a upper limit of `max_ = 2^31 - 1`.
 */
template<typename Integer_, Integer_ max_>
struct Bounded {
    static_assert(std::is_integral<Integer_>::value);
    static_assert(max_ >= 0);

    /**
     * @param x Value of the integer.
     * This should be no greater than `max_`.
     */
    constexpr Bounded(Integer_ x) : value(x) {
        assert(x <= max_);
    }

    /**
     * Type of the integer.
     */
    typedef Integer_ Integer;

    /**
     * Maximum value of the integer, known at compile time.
     * This will be non-negative.
     */
    static constexpr Integer_ max = max_;

    /**
     * Value of the integer.
     */
    Integer_ value;
};

/**
 * Default case for checking whether `Value_` is a `Bounded`.
 *
 * @tparam Value_ Type to be queried.
 */
template<typename Value_>
struct is_Bounded {
    /**
     * False, `Value_` is not a `Bounded`.
     */
    static constexpr bool value = false;
};

/**
 * Positive case for checking whether a class instace is a `Bounded`.
 *
 * @tparam Integer_ See documentation for `Bounded`.
 * @tparam max_ See documentation for `Bounded`.
 */
template<typename Integer_, Integer_ max_>
struct is_Bounded<Bounded<Integer_, max_> > {
    /**
     * True, this class is a `Bounded`.
     */
    static constexpr bool value = true;
};

/** 
 * @cond
 */
// Avoid issues with constness.
template<typename Integer_, Integer_ max_>
struct is_Bounded<const Bounded<Integer_, max_> > {
    static constexpr bool value = true;
}; 

// Declarations below.
template<typename Value_>
constexpr auto get_value(const Value_);

template<typename Value_>
constexpr auto get_max();

template<typename Left_, typename Right_>
constexpr bool is_greater_than(Left_, Right_);

// Internal function.
template<bool safe_, typename Max_, Max_ new_max_, typename Value_>
constexpr auto set_bound_internal(Value_ x) {
    if constexpr(is_greater_than(get_max<Value_>(), new_max_)) {
        const auto val = get_value(x);
        if constexpr(safe_) {
            if (is_greater_than(val, new_max_)) {
                throw std::domain_error("input value exceeds the declared upper bound");
            }
        }
        return Bounded<I<decltype(val)>, new_max_>(val);
    } else {
        return x; 
    }
}
/** 
 * @endcond
 */

/**
 * @tparam Max_ Integer type of the new compile-time maximum.
 * @tparam new_max_ The new compile-time maximum.
 * @tparam Value_ Integer type, `Bounded` or `Constant`.
 * @param x Representation of an integer value. 
 * @return `x` if its maximum value is less than or equal to `new_max_`.
 * Otherwise, an `Bounded` that declares this constraint.
 *
 * @throw std::domain_error Thrown if the value of `x` is greater than `new_max_`.
 *
 * If `x` is known to be less than or equal to `max_`,
 * users can call `set_bound_unsafe()` to avoid the check. 
 */
template<typename Max_, Max_ new_max_, typename Value_>
constexpr auto set_bound(Value_ x) {
    return set_bound_internal<true, Max_, new_max_>(x);
}

/**
 * Variant of `set_bound()` that uses the maximum value of an integer type.
 *
 * @tparam MaxType_ Integer type of the new compile-time maximum.
 * @tparam Value_ Integer type, `Bounded` or `Constant`.
 * @param x Representation of an integer value. 
 * @return `x` if its maximum value is less than or equal to that of `MaxType_`.
 * Otherwise, an `Bounded` that declares this constraint.
 *
 * If `x` is known to be less than or equal to `max_`,
 * users can call `set_bound_by_type_unsafe()` to avoid the check. 
 */
template<typename MaxType_, typename Value_>
constexpr auto set_bound_by_type(Value_ x) {
    return set_bound<MaxType_, get_max<MaxType_>()>(x);
}

/**
 * Variant of `set_bound()` that assumes `x <= new_max_` and does not throw exceptions.
 *
 * @tparam Max_ Integer type of the new compile-time maximum.
 * @tparam new_max_ The new compile-time maximum.
 * @tparam Value_ Integer type, `Bounded` or `Constant`.
 * @param x Representation of an integer value. 
 * This should be no greater than `new_max_`.
 * @return `x` if its maximum value is less than or equal to `new_max_`.
 * Otherwise, an `Bounded` that declares this constraint.
 */
template<typename Max_, Max_ new_max_, typename Value_>
constexpr auto set_bound_unsafe(Value_ x) {
    return set_bound_internal<false, Max_, new_max_>(x);
}

/**
 * Variant of `set_bound_unsafe()` that uses the maximum value of an integer type.
 *
 * @tparam MaxType_ Integer type of the new compile-time maximum.
 * @tparam Value_ Integer type, `Bounded` or `Constant`.
 * @param x Representation of an integer value. 
 * This should be less than or equal to the maximum value of `MaxType_`.
 * @return `x` if its maximum value is less than or equal to that of `MaxType_`.
 * Otherwise, an `Bounded` that declares this constraint.
 */
template<typename MaxType_, typename Value_>
constexpr auto set_bound_by_type_unsafe(Value_ x) {
    return set_bound<MaxType_, get_max<MaxType_>()>(x);
}

/** 
 * @cond
 */
// Back-compatibility.
template<typename Integer_, Integer_ max_>
using Attestation = Bounded<Integer_, max_>;

template<typename ... Args_>
using is_Attestation = is_Bounded<Args_...>;

template<typename Max_, Max_ new_max_, typename Value_>
constexpr auto attest_max(Value_ x) { return set_bound<Max_, new_max_, Value_>(x); }

template<typename MaxType_, typename Value_>
constexpr auto attest_max_by_type(Value_ x) { return set_bound_by_type<MaxType_, Value_>(x); }

template<typename Value_>
Value_ attest_gez(Value_ x) { return x; }
/** 
 * @endcond
 */

}

#endif
