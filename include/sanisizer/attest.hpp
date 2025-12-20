#ifndef SANISIZER_ATTEST_HPP
#define SANISIZER_ATTEST_HPP

#include <limits>
#include <type_traits>
#include <cassert>

namespace sanisizer {

template<typename Type_, bool is_unsigned_, Type_ limit_>
struct Attestation {
    static_assert(std::is_integral<Type_>::value);
    static_assert(!(std::is_unsigned<Type_>::value && !is_unsigned_));

    constexpr Attestation(Type_ value) : value(value) {
        assert(value <= limit_);
        if constexpr(!std::is_unsigned<Type_>::value && is_unsigned_) {
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

    typedef Type_ Type;

    static constexpr bool is_unsigned = is_unsigned_;

    static constexpr Type limit = limit_;

    Type_ value;
};

template<typename Value_>
constexpr auto get_value(Value_ val) {
    if constexpr(std::is_integral<Value_>::value) {
        return val;
    } else {
        return val.value;
    }
}

template<typename Value_>
constexpr bool is_unsigned(const Value_&) {
    if constexpr(std::is_integral<Value_>::value) {
        return std::is_unsigned<Value_>::value;
    } else {
        return Value_::is_unsigned;
    }
}

template<typename Value_>
constexpr auto attest_unsigned(Value_ val) {
    if constexpr(std::is_integral<Value_>::value) {
        if constexpr(std::is_unsigned<Value_>::value) {
            return val;
        } else {
            return Attestation<Value_, true, std::numeric_limits<Value_>::max()>(val);
        }
    } else {
        if constexpr(Value_::is_unsigned) {
            return val;
        } else {
            return Attestation<typename Value_::Type, true, Value_::limit>(val.value);
        }
    }
}

template<typename Value_>
constexpr auto get_limit(const Value_&) {
    if constexpr(std::is_integral<Value_>::value) {
        return std::numeric_limits<Value_>::max();
    } else {
        return Value_::limit;
    }
}

template<typename Limit_, Limit_ new_limit_, typename Value_>
constexpr auto attest_limit(Value_ val) {
    constexpr auto unsigned_new_limit = static_cast<typename std::make_unsigned<Limit_>::type>(new_limit_);
    if constexpr(std::is_integral<Value_>::value) {
        constexpr auto max_value = static_cast<typename std::make_unsigned<Value_>::type>(std::numeric_limits<Value_>::max());
        if constexpr(max_value <= unsigned_new_limit) {
            return val; 
        } else {
            return Attestation<Value_, std::is_unsigned<Value_>::value, static_cast<Value_>(new_limit_)>(val);
        }
    } else {
        typedef typename Value_::Type WrappedType;
        constexpr auto max_value = static_cast<typename std::make_unsigned<WrappedType>::type>(Value_::limit);
        if constexpr(max_value <= unsigned_new_limit) {
            return val;
        } else {
            return Attestation<WrappedType, Value_::is_unsigned, static_cast<WrappedType>(new_limit_)>(val.value);
        }
    }
}

template<typename Limit_, typename Value_>
constexpr auto attest_limit_by_type(Value_ val) {
    return attest_limit<Limit_, std::numeric_limits<Limit_>::max()>(val);
}

}

#endif
