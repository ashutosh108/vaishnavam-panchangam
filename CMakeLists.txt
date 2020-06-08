cmake_minimum_required(VERSION 3.13.0 FATAL_ERROR)

project(vaishnavam-panchangam VERSION 0.1 LANGUAGES C CXX)
option(VP_BUILD_STATIC_EXECUTABLE "build static executable (no .dll or .so dependencies like MSVCRT or libc++)" ON)
option(VP_BUILD_QT_GUI "build Qt GUI" ON)

include(cmake/common.cmake)

# static build options should be on the top of CMakeLists.txt file because
# we use add_compile_options which modifies COMPILE_OPTIONS
# which in turn affect default target compile options *when the target is declared*.
# So we better do it before we declare any targets.
if (VP_BUILD_STATIC_EXECUTABLE)
    change_c_cxx_flags_to_static()
endif()

set(VP_CLI_EXE ${PROJECT_NAME}-cli)

add_executable(${VP_CLI_EXE} src/main.cpp)
target_sources(${VP_CLI_EXE} PRIVATE NOTES.txt)
target_include_directories(${VP_CLI_EXE} PRIVATE src)
target_link_libraries(${VP_CLI_EXE} PRIVATE swe)
if (VP_BUILD_STATIC_EXECUTABLE)
    make_project_static(${VP_CLI_EXE})
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
# When INSTALL is defined, tz doesn't require <wordexp.h> which allows us
# to compile on some platofms. Actual value of INSTALL doesn't matter so much
# as we override it with set_install() on startup.
#
# HAS_REMOTE_API=0 is a hack to avoid requiring curl (and we don't want to require curl yet).
target_compile_definitions(tz PRIVATE HAS_REMOTE_API=0 INSTALL=.)
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
    src/text-interface.cpp
    src/calc-error.cpp
)
target_include_directories(swe PRIVATE vendor/sweph/src src)
target_link_libraries(swe PRIVATE sweph PUBLIC date::tz tl-expected)

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
    tests/test-text-interface.cpp
    tests/html-parser.cpp tests/test-html-parser.cpp
    tests/html-table-parser.cpp tests/test-html-table-parser.cpp
    tests/test-precalculated.cpp
)
target_include_directories(test-main PRIVATE ${PROJECT_SOURCE_DIR}/src)
target_include_directories(test-main PRIVATE vendor/catch2/single_include/catch2 vendor/tinyfsm/include)
target_link_libraries(test-main PRIVATE sweph swe date::date)

enable_testing()
add_test(test-main test-main)

add_custom_target(
    wasmdeploy
    COMMAND
    ${CMAKE_COMMAND} --build . --target ${VP_CLI_EXE}
    COMMAND
    # "$<TARGET_FILE_DIR:vaishnavam-panchangam>/vaishnavam-panchangam.{js,wasm}"
    rsync vaishnavam-panchangam.js vaishnavam-panchangam.wasm ../public_html/index.html 192.168.88.172:public_html/vaishnavam-panchangam/
)

# [1910..1919] is VS2017. Avoid /WX for catch2 because catch2
# emits "no '4800' warning" warnings in MSVC2017 and the maintainer refuses to fix that.
# Reference: https://github.com/catchorg/Catch2/issues/1787
# For other MSVC versions it's OK to keep the /WX flag.
if (MSVC AND (MSVC_VERSION GREATER 1909) AND (MSVC_VERSION LESS 1920))
    target_compile_options(test-main PRIVATE ${WARN_FLAGS_NO_WX})
else()
    target_compile_options(test-main PRIVATE ${WARN_FLAGS})
endif()

target_compile_options(${VP_CLI_EXE} PRIVATE ${WARN_FLAGS})

add_custom_command(TARGET ${VP_CLI_EXE} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_CURRENT_SOURCE_DIR}/vendor/tzdata ${CMAKE_BINARY_DIR}/tzdata)
add_custom_command(TARGET test-main POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_CURRENT_SOURCE_DIR}/vendor/tzdata ${CMAKE_BINARY_DIR}/tzdata)

file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/eph)
file(DOWNLOAD https://github.com/ashutosh108/eph/raw/master/sepl_18.se1 ${CMAKE_BINARY_DIR}/eph/sepl_18.se1 EXPECTED_HASH MD5=76235ef7e2365da3e1e4492d5c3f7801)
file(DOWNLOAD https://github.com/ashutosh108/eph/raw/master/semo_18.se1 ${CMAKE_BINARY_DIR}/eph/semo_18.se1 EXPECTED_HASH MD5=7d67f3203b5277865235529ed26eaf19)

install(TARGETS ${VP_CLI_EXE} DESTINATION .)
install(DIRECTORY ${CMAKE_BINARY_DIR}/eph ${CMAKE_BINARY_DIR}/tzdata DESTINATION .)
add_custom_target(
    zip
    COMMAND
    ${CMAKE_COMMAND} --build . --target ${VP_CLI_EXE}
    COMMAND
    ${CMAKE_COMMAND} --install . --prefix ${PROJECT_NAME} --config ${CMAKE_BUILD_TYPE}
    COMMAND
    ${CMAKE_COMMAND} -E tar cvf ${PROJECT_NAME}.zip --format=zip ${PROJECT_NAME}
)

include(CPack)

if (VP_BUILD_QT_GUI)
    add_subdirectory(gui-qt)
endif()

# GCC 7.x and 8.x: link with stdc++fs library
if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU" AND NOT(CMAKE_CXX_COMPILER_VERSION VERSION_LESS 7.0) AND (CMAKE_CXX_COMPILER_VERSION VERSION_LESS 9.0))
    target_link_libraries(test-main PRIVATE stdc++fs)
    target_link_libraries(${VP_CLI_EXE} PRIVATE stdc++fs)
endif()

# address sanitizer with GCC only works in linux and MacOS (not windows)
if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU" AND UNIX)
    set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -fsanitize=address")
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -fsanitize=address")
    set(CMAKE_EXE_LINKER_FLAGS_DEBUG "${CMAKE_EXE_LINKER_FLAGS_DEBUG} -fsanitize=address")
endif()

# gcc 7: define VP_USE_EXPERIMENTAL_FILESYSTEM
if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU" AND NOT(CMAKE_CXX_COMPILER_VERSION VERSION_LESS 7.0) AND (CMAKE_CXX_COMPILER_VERSION VERSION_LESS 8.0))
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DVP_USE_EXPERIMENTAL_FILESYSTEM")
endif()


find_package(Git)
if(GIT_FOUND AND EXISTS "${CMAKE_SOURCE_DIR}/.git")
    EXECUTE_PROCESS(
        COMMAND ${GIT_EXECUTABLE} rev-parse --short HEAD
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
        OUTPUT_VARIABLE VP_REV
        OUTPUT_STRIP_TRAILING_WHITESPACE)
    EXECUTE_PROCESS(
        COMMAND ${GIT_EXECUTABLE} log -1 --format=%cd --date=format:%Y-%m-%d
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
        OUTPUT_VARIABLE VP_COMPILATION_DATE
        OUTPUT_STRIP_TRAILING_WHITESPACE)
else()
    set(VP_REV "unknown")
    string(TIMESTAMP VP_COMPILATION_DATE "%Y-%m-%d" UTC)
endif()
set(VP_VERSION "preview ${VP_COMPILATION_DATE} rev ${VP_REV}")
message(STATUS "VP version ${VP_VERSION}")
target_compile_definitions(swe PRIVATE VP_VERSION="${VP_VERSION}")

add_library(tl-expected INTERFACE)
target_include_directories(tl-expected INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/vendor/expected/include)
