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

// Back-compatibility.
template<typename Integer_, Integer_ max_>
using Attestation = Bounded<Integer_, max_>;

template<typename ... Args_>
using is_Attestation = is_Bounded<Args_...>;

template<typename Value_>
Value_ attest_gez(Value_ x) { return x; }
/** 
 * @endcond
 */

}

#endif
