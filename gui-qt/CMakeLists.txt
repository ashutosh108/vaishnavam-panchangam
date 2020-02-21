cmake_minimum_required(VERSION 3.5)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(VP_QT_GUI_EXE vaishnavam-panchangam)

project(gui-qt LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)


if (MSVC)
    # /MT means include msvcrt as static library in the .exe
    add_compile_options(/MT)
endif()

# QtCreator supports the following variables for Android, which are identical to qmake Android variables.
# Check http://doc.qt.io/qt-5/deployment-android.html for more information.
# They need to be set before the find_package(Qt5 ...) call.

#if(ANDROID)
#    set(ANDROID_PACKAGE_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/android")
#    if (ANDROID_ABI STREQUAL "armeabi-v7a")
#        set(ANDROID_EXTRA_LIBS
#            ${CMAKE_CURRENT_SOURCE_DIR}/path/to/libcrypto.so
#            ${CMAKE_CURRENT_SOURCE_DIR}/path/to/libssl.so)
#    endif()
#endif()

#LinguistTools 
find_package(Qt5 COMPONENTS Widgets Core REQUIRED)

set(TS_FILES gui-qt_en_US.ts)

if(ANDROID)
  add_library(${VP_QT_GUI_EXE} SHARED
    main.cpp
    mainwindow.cpp
    mainwindow.h
    mainwindow.ui
#    ${TS_FILES}
  )
else()
  add_executable(${VP_QT_GUI_EXE}
    main.cpp
    mainwindow.cpp
    mainwindow.h
    mainwindow.ui
    #${TS_FILES}
  )
endif()

#add_definitions(-DQT_STATICPLUGIN)
target_link_libraries(${VP_QT_GUI_EXE} PRIVATE
    Qt5::Widgets
    swe
)
target_include_directories(${VP_QT_GUI_EXE} PRIVATE ../src)

#qt5_create_translation(QM_FILES ${CMAKE_SOURCE_DIR} ${TS_FILES})
install(TARGETS ${VP_QT_GUI_EXE} DESTINATION .)
set_target_properties(${VP_QT_GUI_EXE} PROPERTIES WIN32_EXECUTABLE YES)

if (MSVC)
        target_link_options(${VP_QT_GUI_EXE} PRIVATE /INCREMENTAL:NO /NODEFAULTLIB:MSVCRT)
else()
    if (WIN32)
        target_link_options(${VP_QT_GUI_EXE} PRIVATE -static)
    endif()
endif()

# GCC 7.x and 8.x: link with stdc++fs library
if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU" AND NOT(CMAKE_CXX_COMPILER_VERSION VERSION_LESS 7.0) AND (CMAKE_CXX_COMPILER_VERSION VERSION_LESS 9.0))
    target_link_libraries(${VP_QT_GUI_EXE} PRIVATE stdc++fs)
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
