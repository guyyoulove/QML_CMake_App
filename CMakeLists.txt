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
          xlsxabstractooxmlfile.h xlsxabstractooxmlfile_p.h xlsxabstractsheet.h xlsxabstractsheet_p.h xlsxcell.h xlsxcell_p.h xlsxcellformula.h xlsxcellformula_p.h xlsxcelllocation.h xlsxcellrange.h xlsxcellreference.h xlsxchart.h xlsxchart_p.h xlsxchartsheet.h xlsxchartsheet_p.h xlsxcolor_p.h xlsxconditionalformatting.h xlsxconditionalformatting_p.h xlsxcontenttypes_p.h xlsxdatavalidation.h xlsxdatavalidation_p.h xlsxdatetype.h xlsxdocpropsapp_p.h xlsxdocpropscore_p.h xlsxdocument.h xlsxdocument_p.h xlsxdrawing_p.h xlsxdrawinganchor_p.h xlsxformat.h xlsxformat_p.h xlsxglobal.h xlsxmediafile_p.h xlsxnumformatparser_p.h xlsxrelationships_p.h xlsxrichstring.h xlsxrichstring_p.h xlsxsharedstrings_p.h xlsxsimpleooxmlfile_p.h xlsxstyles_p.h xlsxtheme_p.h xlsxutility_p.h xlsxworkbook.h xlsxworkbook_p.h xlsxworksheet.h xlsxworksheet_p.h xlsxzipreader_p.h xlsxzipwriter_p.h
          xlsxabstractooxmlfile.cpp xlsxabstractsheet.cpp xlsxcell.cpp xlsxcellformula.cpp xlsxcelllocation.cpp xlsxcellrange.cpp xlsxcellreference.cpp xlsxchart.cpp xlsxchartsheet.cpp xlsxcolor.cpp xlsxconditionalformatting.cpp xlsxcontenttypes.cpp xlsxdatavalidation.cpp xlsxdatetype.cpp xlsxdocpropsapp.cpp xlsxdocpropscore.cpp xlsxdocument.cpp xlsxdrawing.cpp xlsxdrawinganchor.cpp xlsxformat.cpp xlsxmediafile.cpp xlsxnumformatparser.cpp xlsxrelationships.cpp xlsxrichstring.cpp xlsxsharedstrings.cpp xlsxsimpleooxmlfile.cpp xlsxstyles.cpp xlsxtheme.cpp xlsxutility.cpp xlsxworkbook.cpp xlsxworksheet.cpp xlsxzipreader.cpp xlsxzipwriter.cpp
          xlsxcolor.cpp

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
include_directories(${Qt${QT_VERSION_MAJOR}Quick_PRIVATE_INCLUDE_DIRS})
install(TARGETS BMS
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR})

if(QT_VERSION_MAJOR EQUAL 6)
    qt_import_qml_plugins(BMS)
    qt_finalize_executable(BMS)
endif()
