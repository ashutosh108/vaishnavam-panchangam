cmake_minimum_required(VERSION 3.5)

set(VP_QT_GUI_EXE vaishnavam-panchangam)

project(gui-qt LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)


option(VP_ARBITRARY_LOCATION_SELECTOR "use new location selector for arbitrary coordinate input (temporary option)" OFF)

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
message(STATUS "Qt5 ${Qt5_VERSION} ${Qt5_DIR}")

set(TS_FILES gui-qt_en_US.ts)

add_library(mainwindow STATIC
    mainwindow.cpp
    mainwindow.h
    mainwindow.ui
    htmlbrowser.cpp
    latitude-edit.cpp
    edit-custom-dates.cpp
)
#target_include_directories(mainwindow PUBLIC ../src)
target_link_libraries(mainwindow PUBLIC
    Qt5::Widgets
    swe
    fmt::fmt
)
if(ANDROID)
  add_library(${VP_QT_GUI_EXE} SHARED
    main.cpp
#    ${TS_FILES}
  )
else()
  add_executable(${VP_QT_GUI_EXE}
    main.cpp
    #${TS_FILES}
  )
endif()

#add_definitions(-DQT_STATICPLUGIN)
target_link_libraries(${VP_QT_GUI_EXE} PRIVATE
    mainwindow
)
target_include_directories(${VP_QT_GUI_EXE} PRIVATE ../src)
if (VP_ARBITRARY_LOCATION_SELECTOR)
    target_compile_definitions(mainwindow PRIVATE VP_ARBITRARY_LOCATION_SELECTOR)
endif()

#qt5_create_translation(QM_FILES ${CMAKE_SOURCE_DIR} ${TS_FILES})
install(TARGETS ${VP_QT_GUI_EXE} DESTINATION .)
set_target_properties(${VP_QT_GUI_EXE} PROPERTIES WIN32_EXECUTABLE YES)

make_project_static(${VP_QT_GUI_EXE})

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

# cmp0076: The target_sources() command converts relative paths to absolute.
cmake_policy(SET CMP0076 NEW)

# cmp0079: target_link_libraries() allows use with targets in other directories.
cmake_policy(SET CMP0079 NEW)

target_sources(test-main PRIVATE mainwindow.test.cpp)
target_sources(test-main PRIVATE edit-custom-dates.test.cpp)
target_link_libraries(test-main PRIVATE mainwindow)
