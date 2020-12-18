macro(common_init)
    message(STATUS "Generator: ${CMAKE_GENERATOR}")
    set(CMAKE_CXX_STANDARD 17)
    set(CMAKE_CXX_STANDARD_REQUIRED ON)
    set(CMAKE_CXX_EXTENSIONS OFF)

    set(C_CXX_FLAGS_VARS
        CMAKE_CXX_FLAGS CMAKE_C_FLAGS
        CMAKE_CXX_FLAGS_DEBUG CMAKE_C_FLAGS_DEBUG
        CMAKE_CXX_FLAGS_RELEASE CMAKE_C_FLAGS_RELEASE
        CMAKE_CXX_FLAGS_MINSIZEREL CMAKE_C_FLAGS_MINSIZEREL
        CMAKE_CXX_FLAGS_RELWITHDEBINFO CMAKE_C_FLAGS_RELWITHDEBINFO)

    # Explicitly set build type to "Debug" by default.
    # In this way linux builds (having empty type by default) will be consistent
    # with windows builds (having "Debug" by default).
    set(CMAKE_BUILD_TYPE Debug CACHE STRING "Build type")

    if(MSVC)
        set(WARN_FLAGS_NO_WX ${WARN_FLAGS} /permissive-
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
            endif()
            if (NOT(CMAKE_CXX_COMPILER_VERSION VERSION_LESS 4.6))
                set(WARN_FLAGS ${WARN_FLAGS} -Wdouble-promotion)
            # GCC >= 4.8
            endif()
            if (NOT(CMAKE_CXX_COMPILER_VERSION VERSION_LESS 4.8))
                set(WARN_FLAGS ${WARN_FLAGS} -Wuseless-cast)
            endif()
            # GCC >= 4.9
            if (NOT(CMAKE_CXX_COMPILER_VERSION VERSION_LESS 4.9))
                set(WARN_FLAGS ${WARN_FLAGS} -fdiagnostics-color)
            endif()
            # GCC >= 6.0
            if (NOT(${CMAKE_CXX_COMPILER_VERSION} VERSION_LESS 6.0))
                set(WARN_FLAGS ${WARN_FLAGS}
                    -Wmisleading-indentation
                    -Wduplicated-cond
                    -Wnull-dereference
                )
            endif()
            # GCC >= 7.0
            if (NOT(${CMAKE_CXX_COMPILER_VERSION} VERSION_LESS 7.0))
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
            endif()
            # CLANG >= 3.8
            if (NOT(${CMAKE_CXX_COMPILER_VERSION} VERSION_LESS 3.8))
                set(WARN_FLAGS ${WARN_FLAGS} -Wdouble-promotion)
            endif()
            # Clang, all versions
            set(WARN_FLAGS ${WARN_FLAGS} -Wsign-conversion)
        endif()
        # Common warnings understood by both GCC and Clang, including old versions.
        if (CMAKE_CXX_COMPILER_ID MATCHES "^GNU|Clang$")
            set(WARN_FLAGS ${WARN_FLAGS}
                -Wall -Wextra
                -Werror
                -Wnon-virtual-dtor -pedantic
                -Wold-style-cast -Wcast-align -Wunused -Woverloaded-virtual
                -Wconversion
                -Wformat=2)
        endif()
        SET(WARN_FLAGS_NO_WX ${WARN_FLAGS})
    endif()
endmacro()

common_init()

macro(replace_c_cxx_flags from to)
    foreach(flag_var ${C_CXX_FLAGS_VARS})
        if(${flag_var} MATCHES ${from})
            string(REGEX REPLACE ${from} ${to} ${flag_var} "${${flag_var}}")
        endif()
    endforeach()
endmacro()

macro(change_c_cxx_flags_to_static)
    if (MSVC)
        replace_c_cxx_flags("/MD" "/MT")
    endif()
endmacro()

function(make_project_static project_name)
    if (MSVC)
        target_link_options(${project_name} PRIVATE /INCREMENTAL:NO /NODEFAULTLIB:MSVCRT)
    else()
        # Add "-static" option only for win32 because in linux glibc -pthread
        # (required for std::call_once used by tz) and -static are incompatible.
        # It works fine for gcc in Windows, though.
        if (WIN32)
            target_link_options(${project_name} PRIVATE -static)
        endif()
    endif()
    if(UNIX)
        set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -static")
        target_link_options(${project_name} PRIVATE -static-libgcc -static-libstdc++)
    endif()
endfunction()
