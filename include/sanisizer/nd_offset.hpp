#ifndef SANISIZER_ND_OFFSET_HPP
#define SANISIZER_ND_OFFSET_HPP

#include <type_traits>
#include <limits>
#include <stdexcept>
#include <cstddef>
#include <utility>

/**
 * @file nd_offset.hpp
 * @brief Offsets for accessing N-dimensional arrays.
 */

namespace sanisizer {

/**
 * @cond
 */
template<typename Size_>
Size_ nd_offset_internal(Size_ extent, Size_ pos) {
    return extent * pos;
}

template<typename Size_, typename... MoreArgs_>
Size_ nd_offset_internal(Size_ extent, Size_ pos, MoreArgs_... more_args) {
    return (pos + nd_offset_internal<Size_>(more_args...)) * extent;
}
/**
 * @endcond
 */

/**
 * Compute offsets for accessing elements in a flattened N-dimensional array (for N > 1).
 * The first dimension is assumed to be the fastest-changing, followed by the second dimension, and so on.
 * 
 * @tparam Size_ Integer type to represent the size of the flattened array.
 * @tparam First_ Integer type to represent the index on or extent of the first dimension.
 * It is assumed that this can be safely cast to `Size_`, as overflow checks should have been performed during array allocation, e.g., via `product()`.
 * @tparam Second_ Integer type to represent the index on or extent of the second dimension.
 * It is assumed that this can be safely cast to `Size_`, as overflow checks should have been performed during array allocation, e.g., via `product()`.
 * @tparam Remaining_ Additional arguments for further dimensions.
 * It is assumed that all types can be safely cast to `Size_`, as overflow checks should have been performed during array allocation, e.g., `product()`.
 *
 * @param x1 Position on the first dimension.
 * @param extent1 Extent of the first dimension.
 * @param x2 Position on the second dimension.
 * @param remaining Additional arguments for further dimensions.
 * These should be `(extentP, xQ)` pairs where `extentP` is the extent of the `P`-th dimension and `xQ` is the position on the `Q = P + 1`-th dimension.
 * For example, for a 3-dimensional array, we would expect an `extent2` and `x3` argument.
 *
 * @return Offset into the array for element `(x1, x2, ...)`.
 */
template<typename Size_, typename First_, typename Second_, typename... Remaining_>
Size_ nd_offset(First_ x1, First_ extent1, Second_ x2, Remaining_... remaining) {
    return static_cast<Size_>(x1) + nd_offset_internal<Size_>(extent1, x2, remaining...);
}

}

#endif

