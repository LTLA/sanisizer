# Sanitize sizes to avoid integer overflow

![Unit tests](https://github.com/LTLA/sanisizer/actions/workflows/run-tests.yaml/badge.svg)
![Documentation](https://github.com/LTLA/sanisizer/actions/workflows/doxygenate.yaml/badge.svg)
[![Codecov](https://codecov.io/gh/LTLA/sanisizer/branch/master/graph/badge.svg?token=UIfGJvpZTi)](https://codecov.io/gh/LTLA/sanisizer)

## Overview

Both `new` and many constructors of STL containers accept an integer that specifies the size of the array/container.
Any user-supplied value will be implicitly cast to the expected type of the argument - `std::size_t` for `new`, and some `size_type` for container constructors. 
In some cases, the implicit cast could silently overflow, resulting in a smaller array/container than expected. 
This library provides a few methods for sanitizing size values so that any overflow results in an error.

## Casting

Given an integer, we use `sanizer::cast()` to convert it to the expected type of the size for our array/container.
This will throw an error if the value of our integer would cause an overflow.

```cpp
long long new_size = 12345;

// Casting it avoids potential for overflow in constructor.
std::vector<double> my_container;
my_container.resize(sanisizer::cast<decltype(my_container.size())>(new_size));

// Same for new, which expects a `std::size_t`.
auto ptr = new double[sanisizer::cast<std::size_t>(new_size)];
```

We could also use `sanisizer::create()`, which creates a new container instance with less of the type-deducation boilerplate: 

```cpp
auto my_container2 = sanisizer::create<std::vector<double> >(new_size);
```

See the [reference documentation](https://ltla.github.io/sanisizer) for more details.

## Capping

A related problem is to cap a value at the maximum of its type, typically when defining defaults for function arguments or class members.
This avoids compile-time overflow (and the associated compiler warnings) and ensures that a sane value is used.
For example, for data member defaults:

```cpp
struct Options {
    // size_t is only guaranteed to hold up to 65535, so this will be the
    // cap if it can't faithfully hold our intended value of 100000.
    std::size_t some_buffer_size = sanisizer::cap<std::size_t>(100000);
};
```

We can also use this inside function arguments:

```cpp
void do_something(long num_chunks = sanisizer::cap<long>(10000000000)) {
    // Ditto for other integer types, which are only guaranteed to hold
    // up to a certain size - for long, this is at least 32 bits.
}
```

## Arithmetic

Sometimes, we need to perform some arithmetic to determine the size of our array/container.
For example, if we are creating an array that is the concatenation of smaller arrays, we would need to add the sizes of the latter.
This summation needs to be checked for overflow using the `sum()` functions.

```cpp
std::vector<long long> individual_sizes{ 123, 456, 789 };
std::size_t combined_size = 0; 
for (auto is : individual_sizes) {
    combined_size = sanisizer::sum<std::size_t>(combined_size, is);
}
```

When allocating contiguous memory for a high-dimensional array, we need to compute the product of the dimension extents.
Again, this calculation can be done safely by using the `product()` functions to check for overflow.

```cpp
std::vector<long long> dimensions{ 123, 456, 789 };
std::size_t combined_size = 1;
for (auto is : individual_sizes) {
    combined_size = sanisizer::product<std::size_t>(combined_size, is);
}
```

## N-dimensional offsets

Consider an N-dimensional array of dimensions `(d1, d2, ..., dN)` that is flattened and stored contiguously in memory.
Let the first dimension be the fastest changing, then the second, and so on.
If we want to access element `(x1, x2, ..., xN)`, we would compute the offset:

```cpp
// For N = 3:
x1 + d1 * (x2 + d2 * (x3));
```

If `x1`, `d1`, `x2`, etc. are of a smaller type than the array/container's size, the intermediate sums and products could overflow,
even if the final offset itself would be representable by the array/container size type. 
To avoid this, we provide the `nd_offset()` function, which casts all inputs to the size type before performing calculations.

```cpp
// Same calculation as above
sanisizer::nd_offset<std::size_t>(x1, d1, x2, d2, x3);
```

The idea is to use `nd_offset()` within tight loops - say, for accessing the 10-th column of a row-major matrix:

```cpp
int c = 10;
for (int r = 0; r < NR; ++r) {
    // Do something with the matrix element at (r, c)
    auto elmt = matrix[sanisizer::nd_offset<std::size_t>(c, NC, r)];
}
```

(One might think that it would be better to compute an initial offset outside of the loop and just add `NC` in each loop iteration to avoid the multiplication.
However, modern compilers - well, Clang and GCC, at least - will optimize out the multiplication so there is no performance penalty in practice.
The above approach is easier to reason about and is more amenable to vectorization as there are no dependencies in the loop body.
Importantly, it avoids overflow from adding `NC` in the final iteration, which could be undefined behavior if the size type is signed.)

## Building projects 

### CMake with `FetchContent`

If you're using CMake, you just need to add something like this to your `CMakeLists.txt`:

```cmake
include(FetchContent)

FetchContent_Declare(
  sanisizer
  GIT_REPOSITORY https://github.com/LTLA/sanisizer
  GIT_TAG master # replace with a pinned version
)

FetchContent_MakeAvailable(sanisizer)
```

Then you can link to **sanisizer** to make the headers available during compilation:

```cmake
# For executables:
target_link_libraries(myexe sanisizer)

# For libaries
target_link_libraries(mylib INTERFACE sanisizer)
```

### CMake with `find_package()`

You can install the library by cloning a suitable version of this repository and running the following commands:

```sh
mkdir build && cd build
cmake .. -DNCLIST_TESTS=OFF
cmake --build . --target install
```

Then you can use `find_package()` as usual:

```cmake
find_package(ltla_sanisizer CONFIG REQUIRED)
target_link_libraries(mylib INTERFACE ltla::sanisizer)
```

### Manual

If you're not using CMake, the simple approach is to just copy the files in the `include/` subdirectory - 
either directly or with Git submodules - and include their path during compilation with, e.g., GCC's `-I`.
