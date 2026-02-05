#ifndef SANISIZER_CREATE_HPP
#define SANISIZER_CREATE_HPP

#include <utility>
#include <limits>

#include "utils.hpp"
#include "attest.hpp"

/**
 * @file create.hpp
 * @brief Safely create and resize containers.
 */

namespace sanisizer {

/**
 * @cond
 */
template<typename Container_, typename = int>
struct effective_size {
    typedef I<decltype(std::declval<Container_>().size())> Type;
};

template<typename Container_>
struct effective_size<Container_, decltype(std::declval<Container_>().end() - std::declval<Container_>().begin(), 0)> {
    typedef I<decltype(std::declval<Container_>().size())> Size;
    typedef I<decltype(std::declval<Container_>().end() - std::declval<Container_>().begin())> Ptrdiff;
    typedef typename std::conditional<(std::numeric_limits<Size>::max() > std::numeric_limits<Ptrdiff>::max()), Ptrdiff, Size>::type Type;
};
/**
 * @endcond
 */

/**
 * Effective size type of a container, defined as the smaller of the type of `size()` and the type of the difference between the `begin()` and `end()` iterators.
 * This ensures that the size of the container can be safely used in the container's iterator arithmetic. 
 * For containers without `begin()` or `end()` methods that return random access iterators, the effective size type is just that of `size()`.
 */
template<class Container_>
using EffectiveSizeType = typename effective_size<Container_>::Type;

/**
 * Cast an integer to the effective size type of a container.
 * This protects against overflow when using this integer in the container's constructor or `resize()` method,
 * or when using this integer in any arithmetic on the container's `begin()`/`end()` iterators (if available).
 *
 * @tparam Container_ Container class with a `size()` method and a constructor that accepts the size as the first argument.
 * @tparam Value_ Integer type of the input size.
 *
 * @param x Non-negative value representing the desired container size.
 *
 * @return `x` as the container's size's type.
 * If overflow would occur, an `OverflowError` is raised.
 */
template<typename Container_, typename Value_>
constexpr EffectiveSizeType<Container_> as_effective_size_type(Value_ x) {
    check_negative(x);
    check_overflow<EffectiveSizeType<Container_> >(x);
    return get_value(x);
}

/**
 * @cond
 */
// Soft-deprecated, use as_effective_size_type() instead.
template<class Container_, typename Value_>
constexpr auto as_size_type(Value_ x) {
    return as_effective_size_type<Container_>(x);
}
/**
 * @endcond
 */

/**
 * Create a new container of a specified size.
 * This protects against overflow when casting the integer size to the container's size type, see `as_effective_size_type()` for details.
 *
 * @tparam Container_ Container class with a `size()` method and a constructor that accepts the size as the first argument.
 * @tparam Value_ Integer type of the input size.
 * @tparam Args_ Further arguments to pass to the container's constructor.
 *
 * @param x Non-negative value representing the desired container size.
 * @param args Additional arguments to pass to the `Container_` constructor after the size.
 *
 * @return An instance of the container, constructed to be of size `x`.
 * If overflow would occur, an `OverflowError` is raised.
 */
template<class Container_, typename Value_, typename ... Args_>
Container_ create(Value_ x, Args_&&... args) {
    return Container_(as_size_type<Container_>(x), std::forward<Args_>(args)...);
}

/**
 * Resize a container to the desired size.
 * This protects against overflow when casting the integer size to the container's size type, see `as_effective_size_type()` for details.
 *
 * @tparam Container_ Container class with a `size()` method and a `resize()` method that accepts the size as the first argument.
 * @tparam Value_ Integer type of the input size.
 * @tparam Args_ Further arguments to pass to the container's `resize()` method.
 *
 * @param container An existing instance of the container.
 * This is resized to size `x`.
 * @param x Non-negative value representing the desired container size.
 * If this would overflow the container's size type, an `OverflowError` is raised.
 * @param args Additional arguments to pass to `resize()` after the size.
 */
template<class Container_, typename Value_, typename ... Args_>
void resize(Container_& container, Value_ x, Args_&&... args) {
    container.resize(as_size_type<Container_>(x), std::forward<Args_>(args)...);
}

}

#endif
