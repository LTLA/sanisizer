#ifndef SANISIZER_UTILS_HPP
#define SANISIZER_UTILS_HPP

#include <type_traits>
#include <string>

namespace sanisizer {

template<typename Input_>
using I = std::remove_cv_t<std::remove_reference_t<Input_> >;

template<typename Integer_>
std::string failure() {
    static_assert(std::is_integral<Integer_>::value);
    if constexpr(std::is_unsigned<Integer_>::value) {
        return "wraparound";
    } else {
        return "overflow";
    }
}

}

#endif
