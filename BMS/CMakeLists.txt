cmake_minimum_required(VERSION 3.14)

project(BMS VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

#猜测主要做h和库的目录包含，链接库还需要后面生成bin后进行
find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Core Quick)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Core Quick)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS SerialPort)


file(GLOB PROJECT_SOURCES 
${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp
${CMAKE_CURRENT_SOURCE_DIR}/src/*.qrc)

file(GLOB_RECURSE PROJECT_HEADERS ${CMAKE_CURRENT_SOURCE_DIR}/include/*.h)

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(BMS
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
    )
# Define target properties for Android with Qt 6 as:
#    set_property(TARGET BMS APPEND PROPERTY QT_ANDROID_PACKAGE_SOURCE_DIR
#                 ${CMAKE_CURRENT_SOURCE_DIR}/android)
# For more information, see https://doc.qt.io/qt-6/qt-add-executable.html#target-creation
else()
    if(ANDROID)
        add_library(BMS SHARED
            ${PROJECT_SOURCES}
        )
# Define properties for Android with Qt 5 after find_package() calls as:
#    set(ANDROID_PACKAGE_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/android")
    else()
        add_executable(BMS
          ${PROJECT_SOURCES} ${PROJECT_HEADERS})

    endif()
endif()

link_directories( ${CMAKE_CURRENT_SOURCE_DIR}/../xlsx/build)
target_link_libraries(BMS PRIVATE xlsx)#必须加PRIVATE关键字，需要所有的保持一致关键字，否则报错
#还需要手动copy lib到目录，否则报无法打开xlsx.lib文件，怎么自动化？


target_link_libraries(BMS PRIVATE Qt${QT_VERSION_MAJOR}::Core Qt${QT_VERSION_MAJOR}::Quick)
target_link_libraries(BMS PRIVATE Qt${QT_VERSION_MAJOR}::SerialPort)
# PRIVATE
# 作用: 仅在目标自身中使用。
# 传播属性: 目标本身会使用指定的库或编译选项，但不会将这些库或编译选项传播到依赖它的目标。
# 使用场景: 当某个库或编译选项仅用于构建当前目标时使用。
# PUBLIC
# 作用: 在目标自身和依赖于它的目标中使用。
# 传播属性: 目标自身会使用指定的库或编译选项，所有依赖于它的目标也会继承这些库或编译选项。
# 使用场景: 当某个库或编译选项不仅用于当前目标，还需要传播给依赖于它的目标时使用。
# INTERFACE
# 作用: 仅在依赖于该目标的目标中使用。
# 传播属性: 当前目标本身不会使用这些库或编译选项，但所有依赖于它的目标会继承这些库或编译选项。
# 使用场景: 当某个库或编译选项仅需要在依赖该目标的目标中使用，而当前目标自身不需要时使用。

#target_link_libraries(BMS PRIVATE Qt${QT_VERSION_MAJOR}::QZipReader)
#target_link_libraries(BMS PRIVATE Qt${QT_VERSION_MAJOR}::QZipWriter)

# Qt for iOS sets MACOSX_BUNDLE_GUI_IDENTIFIER automatically since Qt 6.1.
# If you are developing for iOS or macOS you should consider setting an
# explicit, fixed bundle identifier manually though.
if(${QT_VERSION} VERSION_LESS 6.1.0)
  set(BUNDLE_ID_OPTION MACOSX_BUNDLE_GUI_IDENTIFIER com.example.BMS)
endif()
set_target_properties(BMS PROPERTIES
    ${BUNDLE_ID_OPTION}
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

include(GNUInstallDirs)
include_directories(
    ${Qt${QT_VERSION_MAJOR}Quick_PRIVATE_INCLUDE_DIRS}
    ${CMAKE_CURRENT_SOURCE_DIR}/include
     ${CMAKE_CURRENT_SOURCE_DIR}/../xlsx/include

)
install(TARGETS BMS
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR})

if(QT_VERSION_MAJOR EQUAL 6)
    qt_import_qml_plugins(BMS)
    qt_finalize_executable(BMS)
endif()

#set(EXECUTABLE_OUTPUT_PATH ${CMAKE_CURRENT_SOURCE_DIR}../../build)
#set(RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}../../out)
