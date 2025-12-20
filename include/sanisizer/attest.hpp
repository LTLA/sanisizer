#ifndef SANISIZER_ATTEST_HPP
#define SANISIZER_ATTEST_HPP

#include <limits>
#include <type_traits>
#include <cassert>

/**
 * @file attest.hpp
 * @brief Create compile-time attestations.
 */

namespace sanisizer {

/**
 * @brief Attest to additional compile-time properties of an integer.
 *
 * @tparam Type_ Type of the integer.
 * @tparam gez_ Whether the integer is greater than or equal to zero, known at compile time.
 * This should be `true` if `Type_` is unsigned.
 * @tparam max_ Maximum value of the integer, known at compile time.
 * This should be non-negative.
 *
 * `gez_` and `max_` are compile-time attestations to the properties of the integer.
 * This allows developers to specify more constraints on the integer than would otherwise be provided by `Type_`.
 * For example, we could attest that an `int` can only store a non-negative value if `gez_ = true`,
 * or that a `std::size_t` has a upper limit of `max_ = 2^31 - 1`.
 */
template<typename Type_, bool gez_, Type_ max_>
struct Attestation {
    static_assert(std::is_integral<Type_>::value);
    static_assert(!(std::is_unsigned<Type_>::value && !gez_));
    static_assert(max_ >= 0);

    /**
     * @value Value of the integer.
     * This should be no greater than `max_`.
     * If `gez_ = true`, this should be non-negative.
     */
    constexpr Attestation(Type_ value) : value(value) {
        assert(value <= max_);
        if constexpr(!std::is_unsigned<Type_>::value && gez_) {
            assert(value >= 0);
        }
    }

    /**
     * @cond
     */
#ifndef SANISIZER_STRICT_SIGNATURES
    template<typename Other_> 
    Attestation(Other_) = delete;
#endif
    /**
     * @endcond
     */

    /**
     * Type of the integer.
     */
    typedef Type_ Type;

    /**
     * Whether the integer is greater than or equal to zero, known at compile time.
     * This will be `true` if `Type_` is unsigned.
     */
    static constexpr bool gez = gez_;

    /**
     * Maximum value of the integer, known at compile time.
     * This will be non-negative.
     */
    static constexpr Type max = max_;

    /**
     * Value of the integer.
     */
    Type_ value;
};

/**
 * @tparam Value_ Integer or `Attestation`.
 * @param x Integer value or an `Attestation` about an integer.
 * @return `x` if it is already an integer, otherwise `Attestation::value`.
 */
template<typename Value_>
constexpr auto get_value(Value_ x) {
    if constexpr(std::is_integral<Value_>::value) {
        return x;
    } else {
        return x.value;
    }
}

/**
 * @tparam Value_ Integer or `Attestation`.
 * @param x Integer value or an `Attestation` about an integer.
 * @return Whether the value of `x` is known to be greater than or equal to zero at compile time.
 */
template<typename Value_>
constexpr bool get_gez(const Value_&) {
    if constexpr(std::is_integral<Value_>::value) {
        return std::is_unsigned<Value_>::value;
    } else {
        return Value_::gez;
    }
}

/**
 * @tparam Value_ Integer or `Attestation`.
 * @param x Integer value or an `Attestation` about an integer.
 * @return `x` if it is already known to be greater than or equal to zero at compile time;
 * otherwise, an `Attestation` that attests to this constraint.
 */
template<typename Value_>
constexpr auto attest_gez(Value_ val) {
    if constexpr(std::is_integral<Value_>::value) {
        if constexpr(std::is_unsigned<Value_>::value) {
            return val;
        } else {
            return Attestation<Value_, true, std::numeric_limits<Value_>::max()>(val);
        }
    } else {
        if constexpr(Value_::gez) {
            return val;
        } else {
            return Attestation<typename Value_::Type, true, Value_::max>(val.value);
        }
    }
}

/**
 * @tparam Value_ Integer or `Attestation`.
 * @param x Integer value or an `Attestation` about an integer.
 * @return The maximum value of an integer that is known at compile time.
 */
template<typename Value_>
constexpr auto get_max(const Value_&) {
    if constexpr(std::is_integral<Value_>::value) {
        return std::numeric_limits<Value_>::max();
    } else {
        return Value_::max;
    }
}

/**
 * @tparam Max_ Integer type of the new compile-time maximum.
 * @tparam new_max_ The new compile-time maximum.
 * @tparam Value_ Integer or `Attestation`.
 * @param x Integer value or an `Attestation` about an integer.
 * @return `x` if it is already known to be less than or equal to `new_max_`, 
 * otherwise, an `Attestation` that attests to this constraint.
 */
template<typename Max_, Max_ new_max_, typename Value_>
constexpr auto attest_max(Value_ val) {
    constexpr auto unsigned_new_limit = static_cast<typename std::make_unsigned<Max_>::type>(new_max_);
    if constexpr(std::is_integral<Value_>::value) {
        constexpr auto max_value = static_cast<typename std::make_unsigned<Value_>::type>(std::numeric_limits<Value_>::max());
        if constexpr(max_value <= unsigned_new_limit) {
            return val; 
        } else {
            return Attestation<Value_, std::is_unsigned<Value_>::value, static_cast<Value_>(new_max_)>(val);
        }
    } else {
        typedef typename Value_::Type WrappedType;
        constexpr auto max_value = static_cast<typename std::make_unsigned<WrappedType>::type>(Value_::max);
        if constexpr(max_value <= unsigned_new_limit) {
            return val;
        } else {
            return Attestation<WrappedType, Value_::gez, static_cast<WrappedType>(new_max_)>(val.value);
        }
    }
}

/**
 * @tparam Max_ Integer type of the new compile-time maximum.
 * @tparam Value_ Integer or `Attestation`.
 * @param x Integer value or an `Attestation` about an integer.
 * @return `x` if it is already known to be less than or equal to the maximum value of `Max_`;
 * otherwise, an `Attestation` that attests to this constraint.
 */
template<typename Max_, typename Value_>
constexpr auto attest_max_by_type(Value_ val) {
    return attest_max<Max_, std::numeric_limits<Max_>::max()>(val);
}

}

#endif
