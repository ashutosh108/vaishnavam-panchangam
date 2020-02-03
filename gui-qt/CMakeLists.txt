cmake_minimum_required(VERSION 3.5)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

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
  add_library(gui-qt SHARED
    main.cpp
    mainwindow.cpp
    mainwindow.h
    mainwindow.ui
#    ${TS_FILES}
  )
else()
  add_executable(gui-qt
    main.cpp
    mainwindow.cpp
    mainwindow.h
    mainwindow.ui
    #${TS_FILES}
  )
endif()

#add_definitions(-DQT_STATICPLUGIN)
target_link_libraries(gui-qt PRIVATE
    Qt5::Widgets
    swe
)
target_include_directories(gui-qt PRIVATE ../src)

#qt5_create_translation(QM_FILES ${CMAKE_SOURCE_DIR} ${TS_FILES})
install(TARGETS ${PROJECT_NAME} DESTINATION .)
set_target_properties(gui-qt PROPERTIES WIN32_EXECUTABLE YES)

if (MSVC)
	target_link_options(${PROJECT_NAME} PRIVATE /INCREMENTAL:NO /NODEFAULTLIB:MSVCRT)
else()
    if (WIN32)
        target_link_options(${PROJECT_NAME} PRIVATE -static)
    endif()
endif()
