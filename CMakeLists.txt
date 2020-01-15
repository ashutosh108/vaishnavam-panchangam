cmake_minimum_required(VERSION 3.1.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

project(vaishnavam-panchangam VERSION 0.1 LANGUAGES C CXX)
option(VP_BUILD_STATIC_EXECUTABLE "build static executable (no .dll or .so dependencies like MSVCRT or libc++)" ON)

# Explicitly set build type to "Debug" by default.
# In this way linux builds (having empty type by default) will be consistent
# with windows builds (having "Debug" by default).
set(CMAKE_BUILD_TYPE Debug CACHE STRING "Build type")

# static build options should be on the top of CMakeLists.txt file because
# we use add_compile_options which modifies COMPILE_OPTIONS
# which in turn affect default target compile options *when the target is declared*.
# So we better do it before we declare any targets.
if (VP_BUILD_STATIC_EXECUTABLE)
    if (MSVC)
        # /MT means include msvcrt as static library in the .exe
        add_compile_options(/MT)
    endif()
endif()

add_executable(${PROJECT_NAME} src/main.cpp)
target_sources(${PROJECT_NAME} PRIVATE NOTES.txt)
target_include_directories(${PROJECT_NAME} PRIVATE src)
target_link_libraries(${PROJECT_NAME} PRIVATE swe)
if (VP_BUILD_STATIC_EXECUTABLE)
    if (MSVC)
        target_link_options(${PROJECT_NAME} PRIVATE /INCREMENTAL:NO /NODEFAULTLIB:MSVCRT)
    else()
        # Add "-static" option only for win32 because in linux glibc -pthread
        # (required for std::call_once used by tz) and -static are incompatible.
        # It works fine for gcc in Windows, though.
        if (WIN32)
            target_link_options(${PROJECT_NAME} PRIVATE -static)
        endif()
    endif()
endif()


add_library(date INTERFACE)
add_library(date::date ALIAS date)
target_include_directories(date INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/vendor/date/include)
# Set HAS_UNCAUGHT_EXCEPTIONS=1 to avoid lots of warnings on gcc-9.2 (and, presumably,
# on other modern C++17 compilers)
target_compile_definitions(date INTERFACE HAS_UNCAUGHT_EXCEPTIONS=1)

add_library(tz STATIC vendor/date/src/tz.cpp)
add_library(date::tz ALIAS tz)
target_link_libraries( tz PUBLIC date::date)
target_compile_definitions(tz PRIVATE HAS_REMOTE_API=0)
# glibc on linux seem to require -pthread for std::call_once. Approximate that with "not win32".
if (NOT WIN32)
    find_package(Threads)
    target_link_libraries(tz PRIVATE Threads::Threads)
endif()


add_library(swe STATIC
    src/juldays_ut.h src/juldays_ut.cpp
    src/swe.h src/swe.cpp
    src/calc.h src/calc.cpp
    src/tithi.h src/tithi.cpp
    src/location.h
    src/vrata.h src/vrata.cpp
    src/vrata_detail.h src/vrata_detail.cpp
    src/paran.h src/paran.cpp
)
target_include_directories(swe PRIVATE vendor/sweph/src src)
target_link_libraries(swe PRIVATE sweph PUBLIC date::tz)

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
    tests/test-juldays_ut.cpp
    tests/test-swe.cpp
    tests/test-calc.cpp
    tests/test-tithi.cpp
    tests/test-location.cpp
    tests/test-date.cpp
    tests/test-vrata.cpp
    tests/test-vrata-detail.cpp
    tests/test-paran.cpp
)
target_include_directories(test-main PRIVATE ${PROJECT_SOURCE_DIR}/src)
target_include_directories(test-main PRIVATE vendor/catch2/single_include/catch2)
target_link_libraries(test-main PRIVATE sweph swe date::date)

enable_testing()
add_test(test-main test-main)

add_custom_target(
    wasmdeploy
    COMMAND
    ${CMAKE_COMMAND} --build . --target ${PROJECT_NAME}
    COMMAND
    # "$<TARGET_FILE_DIR:vaishnavam-panchangam>/vaishnavam-panchangam.{js,wasm}"
    rsync vaishnavam-panchangam.js vaishnavam-panchangam.wasm ../public_html/index.html 192.168.88.172:public_html/vaishnavam-panchangam/
)

add_executable(qwe src/qwe.cpp)
target_link_libraries(qwe PRIVATE date::tz)

if(MSVC)
    set(WARN_FLAGS_NO_WX ${WARN_FLAGS} /permissive- /W4
        /w14242 /w14254 /w14263 /w14265 /w14287 /we4289 /w14296 /w14311
        /w14545 /w14546 /w14547 /w14549 /w14555 /w14619 /w14640 /w14826
        /w14905 /w14906 /w14928)
    set(WARN_FLAGS ${WARN_FLAGS_NO_WX} /WX)
else()
    if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
        # GCC >= 4.3
        if (NOT(CMAKE_CXX_COMPILER_VERSION VERSION_LESS 4.3))
            set(WARN_FLAGS ${WARN_FLAGS} -Wsign-conversion)
        # GCC >= 4.6
        elseif (NOT(CMAKE_CXX_COMPILER_VERSION VERSION_LESS 4.6))
            set(WARN_FLAGS ${WARN_FLAGS} -Wdouble-promotion)
        # GCC >= 4.8
        elseif (NOT(CMAKE_CXX_COMPILER_VERSION VERSION_LESS 4.8))
            set(WARN_FLAGS ${WARN_FLAGS} -Wuseless-cast)
        # GCC >= 6.0
        elseif (NOT(${CMAKE_CXX_COMPILER_VERSION} VERSION_LESS 6.0))
            set(WARN_FLAGS ${WARN_FLAGS}
                -Wmisleading-indentation
                -Wduplicated-cond
                -Wnull-dereference
            )
        # GCC >= 7.0
        elseif (NOT(${CMAKE_CXX_COMPILER_VERSION} VERSION_LESS 7.0))
            set(WARN_FLAGS ${WARN_FLAGS} -Wduplicated-branches)
        endif()
        # GCC, all versions
        set(WARN_FLAGS ${WARN_FLAGS}
            -Wlogical-op
            -Wpedantic)
    elseif(CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
        # CLANG >= 3.2
        if (NOT(${CMAKE_CXX_COMPILER_VERSION} VERSION_LESS 3.2))
            set(WARN_FLAGS ${WARN_FLAGS} -Wpedantic)
        # CLANG >= 3.8
        elseif (NOT(${CMAKE_CXX_COMPILER_VERSION} VERSION_LESS 3.8))
            set(WARN_FLAGS ${WARN_FLAGS} -Wdouble-promotion)
        # CLANG >= 8.0
        elseif (NOT(${CMAKE_CXX_COMPILER_VERSION} VERSION_LESS 8.0))
            # As of now (Nov 2018) -Wlifetime is only present in the
            # special branch of clang (not even in trunk!).
            # So, assume future release 8 (but we don't really know).
            set(WARN_FLAGS ${WARN_FLAGS} -Wlifetime)
        endif()
        # Clang, all versions
        set(WARN_FLAGS ${WARN_FLAGS} -Wsign-conversion)
    endif()
    # Common warnings understood by both GCC and Clang, including old versions.
    if (CMAKE_CXX_COMPILER_ID MATCHES "^GNU|Clang$")
        set(WARN_FLAGS ${WARN_FLAGS}
            -Wall -Wextra -Wshadow
            -Werror
            -Wnon-virtual-dtor -pedantic
            -Wold-style-cast -Wcast-align -Wunused -Woverloaded-virtual
            -Wconversion
            -Wformat=2)
    endif()
    SET(WARN_FLAGS_NO_WX ${WARN_FLAGS})
endif()

# [1910..1919] is VS2017. Avoid /WX for catch2 because catch2
# emits "no '4800' warning" warnings in MSVC2017 and the maintainer refuses to fix that.
# Reference: https://github.com/catchorg/Catch2/issues/1787
# For other MSVC versions it's OK to keep the /WX flag.
if (MSVC AND (MSVC_VERSION GREATER 1909) AND (MSVC_VERSION LESS 1920))
    target_compile_options(test-main PRIVATE ${WARN_FLAGS_NO_WX})
else()
    target_compile_options(test-main PRIVATE ${WARN_FLAGS})
endif()

target_compile_options(${PROJECT_NAME} PRIVATE ${WARN_FLAGS})
target_compile_options(qwe PRIVATE ${WARN_FLAGS})

add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_CURRENT_SOURCE_DIR}/vendor/tzdata ${CMAKE_BINARY_DIR}/tzdata)
add_custom_command(TARGET test-main POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_CURRENT_SOURCE_DIR}/vendor/tzdata ${CMAKE_BINARY_DIR}/tzdata)

file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/eph)
file(DOWNLOAD https://github.com/ashutosh108/eph/raw/master/sepl_18.se1 ${CMAKE_BINARY_DIR}/eph/sepl_18.se1 EXPECTED_HASH MD5=76235ef7e2365da3e1e4492d5c3f7801)
file(DOWNLOAD https://github.com/ashutosh108/eph/raw/master/semo_18.se1 ${CMAKE_BINARY_DIR}/eph/semo_18.se1 EXPECTED_HASH MD5=7d67f3203b5277865235529ed26eaf19)

install(TARGETS ${PROJECT_NAME} DESTINATION .)
install(DIRECTORY ${CMAKE_BINARY_DIR}/eph ${CMAKE_BINARY_DIR}/tzdata DESTINATION .)
add_custom_target(
    zip
    COMMAND
    ${CMAKE_COMMAND} --build . --target ${PROJECT_NAME}
    COMMAND
    ${CMAKE_COMMAND} --install . --prefix ${PROJECT_NAME} --config Debug
    COMMAND
    ${CMAKE_COMMAND} -E tar cvf ${PROJECT_NAME}.zip --format=zip ${PROJECT_NAME}
)
