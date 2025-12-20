include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/7da55820cc32dedd6c1b048f2d4e13fdde5e8237.zip
)

# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)

# Avoid installing GoogleTest when installing this project.
option(INSTALL_GTEST "Enable installation of googletest." OFF)

FetchContent_MakeAvailable(googletest)

set(CMAKE_CXX_STANDARD 17)

enable_testing()

add_executable(libtest 
    src/comparisons.cpp
    src/cast.cpp
    src/create.cpp
    src/arithmetic.cpp
    src/nd_offset.cpp
    src/cap.cpp
    src/ptrdiff.cpp
    src/float.cpp
    src/attest.cpp
)

add_executable(floattest 
    src/float.cpp
)
target_compile_definitions(floattest PRIVATE 
    SANISIZER_FLOAT_FORCE_FREXP=1
    SANISIZER_FLOAT_FORCE_MANUAL=1)

target_link_libraries(libtest gtest_main sanisizer)
target_link_libraries(floattest gtest_main sanisizer)

target_compile_options(libtest PRIVATE -Wall -Wextra -Wpedantic -Werror)
target_compile_options(floattest PRIVATE -Wall -Wextra -Wpedantic -Werror)

set(CODE_COVERAGE OFF CACHE BOOL "Enable coverage testing")
if(CODE_COVERAGE AND CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    target_compile_options(libtest PRIVATE -O0 -g --coverage)
    target_link_options(libtest PRIVATE --coverage)
    target_compile_options(floattest PRIVATE -O0 -g --coverage)
    target_link_options(floattest PRIVATE --coverage)
endif()

# Making the tests discoverable.
include(GoogleTest)
gtest_discover_tests(libtest)
gtest_discover_tests(floattest)
