cmake_minimum_required(VERSION 3.1.0)

set(CMAKE_CXX_STANDARD 17)

project(ekadashi-calculator VERSION 0.1 LANGUAGES C CXX)

add_executable(${PROJECT_NAME} main.cpp)
target_link_libraries(ekadashi-calculator PRIVATE swe)

add_library(swe STATIC
    swe_time.h swe_time.cpp
    swe.h swe.cpp
    calc.h calc.cpp
    tithi.h tithi.cpp
    coord.h coord.cpp
)
target_include_directories(swe PRIVATE vendor/sweph/src)
target_link_libraries(swe PRIVATE sweph)

add_library(sweph STATIC
    vendor/sweph/src/swecl.c
    vendor/sweph/src/swedate.c
    vendor/sweph/src/swehel.c
    vendor/sweph/src/swehouse.c
    vendor/sweph/src/swejpl.c
    vendor/sweph/src/swemmoon.c
    vendor/sweph/src/swemplan.c
    vendor/sweph/src/sweph.c
    vendor/sweph/src/swephlib.c)
target_compile_definitions(sweph PRIVATE NO_SWE_GLP)
if ( MSVC )
    # W4996 is "strncpy may be unsafe, use strncpy_s", etc.
    # But we don't want to change the way sweph is using those functions already.
    target_compile_options(sweph PRIVATE "/wd4996")
endif()

add_executable(test-main
    tests/test-main.cpp
    tests/test-swe_time.cpp
    tests/test-swe.cpp
    tests/test-calc.cpp
    tests/test-tithi.cpp
    tests/test-coord.cpp
)
target_include_directories(test-main PRIVATE ${PROJECT_SOURCE_DIR})
target_include_directories(test-main PRIVATE vendor/catch2/single_include/catch2)
target_link_libraries(test-main PRIVATE sweph swe)
