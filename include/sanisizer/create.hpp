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
struct has_random_access_iterators {
    static constexpr bool value = false;
};

template<typename Container_>
struct has_random_access_iterators<Container_, decltype((void) (std::declval<Container_>().end() - std::declval<Container_>().begin()), 0)> {
    static constexpr bool value = true;
};

template<typename Container_, typename = int>
struct has_data {
    static constexpr bool value = false;
};

template<typename Container_>
struct has_data<Container_, decltype((void) *(std::declval<Container_>().data()), 0)> {
    static constexpr bool value = true;
};

template<typename X_, typename Y_, typename ... Args_>
struct minimum {
    typedef typename std::conditional<
        (static_cast<std::make_unsigned_t<X_> >(std::numeric_limits<X_>::max()) < 
         static_cast<std::make_unsigned_t<Y_> >(std::numeric_limits<Y_>::max())),
        typename minimum<X_, Args_...>::Type,
        typename minimum<Y_, Args_...>::Type
    >::type Type;
};

template<typename X_, typename Y_>
struct minimum<X_, Y_> {
    typedef typename std::conditional<
        (static_cast<std::make_unsigned_t<X_> >(std::numeric_limits<X_>::max()) < 
         static_cast<std::make_unsigned_t<Y_> >(std::numeric_limits<Y_>::max())),
        X_,
        Y_
    >::type Type;
};

template<typename Container_>
auto derive_effective_size() {
    typedef I<decltype(std::declval<Container_>().size())> Size;
    if constexpr(has_random_access_iterators<Container_>::value) {
        typedef I<decltype(std::declval<Container_>().end() - std::declval<Container_>().begin())> Ptrdiff;
        if constexpr(has_data<Container_>::value) {
            return static_cast<typename minimum<Size, Ptrdiff, std::size_t, std::ptrdiff_t>::Type>(0);
        } else {
            return static_cast<typename minimum<Size, Ptrdiff>::Type>(0);
        }
    } else {
        if constexpr(has_data<Container_>::value) {
            return static_cast<typename minimum<Size, std::size_t, std::ptrdiff_t>::Type>(0);
        } else {
            return static_cast<Size>(0);
        }
    }
}
/**
 * @endcond
 */

/**
 * Effective size type of a container, defined as the smallest of:
 *
 * - The type of `size()`, to ensure that any value of the effective size type can be safely used in the container's constructor, `resize()` or `reserve()` methods.
 * - The type of the difference between the `begin()` and `end()` iterators.
 *   This ensures that any value of the effective size type can be safely used in the container's iterator arithmetic. 
 *   Ignored if the container has no `begin()` or `end()` method that returns random access iterators.
 * - `std::ptrdiff_t` and `std::size_t`, if the container exposes an underlying pointer to its contents via the `data()` method.
 *   This ensures that any value of the effective size type can be safely used with such pointers.
 *   Ignored if the container has no `data()` method that returns a pointer.
 *
 * @tparam Container_ Container class with a `size()` method. 
 */
template<class Container_>
using EffectiveSizeType = decltype(derive_effective_size<Container_>());

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
 * @param args Additional arguments to pass to `resize()` after the size.
 */
template<class Container_, typename Value_, typename ... Args_>
void resize(Container_& container, Value_ x, Args_&&... args) {
    container.resize(as_size_type<Container_>(x), std::forward<Args_>(args)...);
}

/**
 * Reserve a container to the desired size.
 * This protects against overflow when casting the integer size to the container's size type, see `as_effective_size_type()` for details.
 *
 * @tparam Container_ Container class with a `size()` method and a `reserve()` method that accepts the size as the first argument.
 * @tparam Value_ Integer type of the input size.
 * @tparam Args_ Further arguments to pass to the container's `reserve()` method.
 *
 * @param container An existing instance of the container.
 * On return, its allocation is set to `x`.
 * @param x Non-negative value representing the desired container size.
 * @param args Additional arguments to pass to `reserve()` after the size.
 */
template<class Container_, typename Value_, typename ... Args_>
void reserve(Container_& container, Value_ x, Args_&&... args) {
    container.reserve(as_size_type<Container_>(x), std::forward<Args_>(args)...);
}

}

#endif
