cmake_minimum_required(VERSION 3.14)

project(sanisizer
    VERSION 0.1.1
    DESCRIPTION "Sanitize sizes to avoid integer overflow"
    LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)

add_library(sanisizer INTERFACE)
add_library(ltla::sanisizer ALIAS sanisizer)

# Switch between include directories depending on whether the downstream is
# using the build directly or is using the installed package.
include(GNUInstallDirs)
target_include_directories(sanisizer
    INTERFACE 
    "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>"
    "$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}/ltla_sanisizer>"
)

# Building the test-related machinery, if we are compiling this library directly.
if(CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME)
    option(SANISIZER_TESTS "Build sanisizer's test suite." ON)
else()
    option(SANISIZER_TESTS "Build sanisizer's test suite." OFF)
endif()

if(SANISIZER_TESTS)
    include(CTest)
    if(BUILD_TESTING)
        add_subdirectory(tests)
    endif() 
endif()

# Installing for find_package.
include(CMakePackageConfigHelpers)

install(DIRECTORY include/
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/ltla_sanisizer)

install(TARGETS sanisizer 
    EXPORT sanisizerTargets)

install(EXPORT sanisizerTargets
    FILE ltla_sanisizerTargets.cmake
    NAMESPACE ltla::
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/ltla_sanisizer)

configure_package_config_file(${CMAKE_CURRENT_SOURCE_DIR}/cmake/Config.cmake.in
    "${CMAKE_CURRENT_BINARY_DIR}/ltla_sanisizerConfig.cmake"
    INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/ltla_sanisizer)

write_basic_package_version_file(
    "${CMAKE_CURRENT_BINARY_DIR}/ltla_sanisizerConfigVersion.cmake"
    COMPATIBILITY SameMajorVersion)

install(FILES "${CMAKE_CURRENT_BINARY_DIR}/ltla_sanisizerConfig.cmake"
    "${CMAKE_CURRENT_BINARY_DIR}/ltla_sanisizerConfigVersion.cmake"
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/ltla_sanisizer)
