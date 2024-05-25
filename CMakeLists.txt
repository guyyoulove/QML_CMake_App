cmake_minimum_required(VERSION 3.14)

project(BMS VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Core Quick)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Core Quick)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS SerialPort)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS SerialPort)

set(PROJECT_SOURCES
        main.cpp
        qml.qrc
)

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
          ${PROJECT_SOURCES}
          batteryinfo.h batteryinfo.cpp
          batteryinfo.h baudinfo.h bms.h comthread.h login.h loginfomodel.h md5.h otherinfomodel.h serialportinfo.h tempinfo.h
          batteryinfo.cpp baudinfo.cpp bms.cpp comthread.cpp login.cpp loginfomodel.cpp main.cpp md5.cpp otherinfomodel.cpp serialportinfo.cpp tempinfo.cpp
          image.qrc
          xlsx_src/xlsxabstractooxmlfile.cpp xlsx_src/xlsxabstractsheet.cpp xlsx_src/xlsxcell.cpp xlsx_src/xlsxcellformula.cpp xlsx_src/xlsxcelllocation.cpp xlsx_src/xlsxcellrange.cpp xlsx_src/xlsxcellreference.cpp xlsx_src/xlsxchart.cpp xlsx_src/xlsxchartsheet.cpp xlsx_src/xlsxcolor.cpp xlsx_src/xlsxconditionalformatting.cpp xlsx_src/xlsxcontenttypes.cpp xlsx_src/xlsxdatavalidation.cpp xlsx_src/xlsxdatetype.cpp xlsx_src/xlsxdocpropsapp.cpp xlsx_src/xlsxdocpropscore.cpp xlsx_src/xlsxdocument.cpp xlsx_src/xlsxdrawing.cpp xlsx_src/xlsxdrawinganchor.cpp xlsx_src/xlsxformat.cpp xlsx_src/xlsxmediafile.cpp xlsx_src/xlsxnumformatparser.cpp xlsx_src/xlsxrelationships.cpp xlsx_src/xlsxrichstring.cpp xlsx_src/xlsxsharedstrings.cpp xlsx_src/xlsxsimpleooxmlfile.cpp xlsx_src/xlsxstyles.cpp xlsx_src/xlsxtheme.cpp xlsx_src/xlsxutility.cpp xlsx_src/xlsxworkbook.cpp xlsx_src/xlsxworksheet.cpp xlsx_src/xlsxzipreader.cpp xlsx_src/xlsxzipwriter.cpp
        )
    endif()
endif()

target_link_libraries(BMS
  PRIVATE Qt${QT_VERSION_MAJOR}::Core Qt${QT_VERSION_MAJOR}::Quick)
target_link_libraries(BMS PRIVATE Qt${QT_VERSION_MAJOR}::SerialPort)
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
include_directories(${Qt${QT_VERSION_MAJOR}Quick_PRIVATE_INCLUDE_DIRS} "./xlsx_src")
install(TARGETS BMS
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR})

if(QT_VERSION_MAJOR EQUAL 6)
    qt_import_qml_plugins(BMS)
    qt_finalize_executable(BMS)
endif()
