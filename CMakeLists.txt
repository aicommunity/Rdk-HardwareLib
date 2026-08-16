cmake_minimum_required(VERSION 3.16)
project(Rdk_HardwareLib LANGUAGES CXX)

set(TARGET_NAME Rdk-HardwareLib.qt)

add_library(${TARGET_NAME} STATIC)

target_include_directories(${TARGET_NAME}
  PUBLIC
    ${CMAKE_SOURCE_DIR}/Rdk/Deploy/Include
    ${CMAKE_SOURCE_DIR}/Libraries/Rdk-HardwareLib/Core
    ${CMAKE_SOURCE_DIR}/Libraries/Rdk-HardwareLib/Core/Transport
    ${CMAKE_SOURCE_DIR}/Libraries/Rdk-HardwareLib/Core/Protocol
    ${CMAKE_SOURCE_DIR}/Libraries/Rdk-HardwareLib/Core/Catalog
)

file(GLOB HW_CORE_SOURCES CONFIGURE_DEPENDS
  ${CMAKE_SOURCE_DIR}/Libraries/Rdk-HardwareLib/Core/*.cpp
  ${CMAKE_SOURCE_DIR}/Libraries/Rdk-HardwareLib/Core/Transport/*.cpp
  ${CMAKE_SOURCE_DIR}/Libraries/Rdk-HardwareLib/Core/Protocol/*.cpp
  ${CMAKE_SOURCE_DIR}/Libraries/Rdk-HardwareLib/Core/Catalog/*.cpp
)

# Explicit list: file(GLOB) is evaluated at configure time; these were added after
# many build trees were generated and must always link into Rdk-HardwareLib.qt.
set(HW_CORE_REQUIRED_SOURCES
  ${CMAKE_SOURCE_DIR}/Libraries/Rdk-HardwareLib/Core/UArduinoPropertyString.cpp
  ${CMAKE_SOURCE_DIR}/Libraries/Rdk-HardwareLib/Core/Transport/UArduinoUploadJob.cpp
  ${CMAKE_SOURCE_DIR}/Libraries/Rdk-HardwareLib/Core/Catalog/UHardwareCatalogPaths.cpp
  ${CMAKE_SOURCE_DIR}/Libraries/Rdk-HardwareLib/Core/Catalog/UHardwareCatalog.cpp
  ${CMAKE_SOURCE_DIR}/Libraries/Rdk-HardwareLib/Core/Catalog/UHardwareSetup.cpp
)

target_sources(${TARGET_NAME} PRIVATE ${HW_CORE_SOURCES} ${HW_CORE_REQUIRED_SOURCES})

target_compile_definitions(${TARGET_NAME} PUBLIC RDK_QT RDK_LIB_CVS_REVISION=0)

if(MSVC)
  target_compile_options(${TARGET_NAME} PRIVATE /utf-8)
endif()

include(${CMAKE_SOURCE_DIR}/cmake/SelectQt.cmake)
target_link_libraries(${TARGET_NAME} PUBLIC
  Nmsdk::Qt::Core
  Nmsdk::Qt::SerialPort
  Nmsdk::Qt::Concurrent
  Threads::Threads
  rdk.static.qt
  Rdk-BasicLib.qt
)

# GUI library
set(GUI_TARGET_NAME Rdk-HardwareLib.gui)
add_library(${GUI_TARGET_NAME} STATIC)

file(GLOB HW_GUI_SOURCES
  ${CMAKE_SOURCE_DIR}/Libraries/Rdk-HardwareLib/GUI/Qt/*.cpp
  ${CMAKE_SOURCE_DIR}/Libraries/Rdk-HardwareLib/GUI/Qt/widgets/*.cpp
)

target_sources(${GUI_TARGET_NAME} PRIVATE ${HW_GUI_SOURCES})

qt_add_resources(HW_GUI_RESOURCES
  ${CMAKE_SOURCE_DIR}/Libraries/Rdk-HardwareLib/GUI/Qt/Resources/hardware_lib.qrc
)
target_sources(${GUI_TARGET_NAME} PRIVATE ${HW_GUI_RESOURCES})

target_include_directories(${GUI_TARGET_NAME} PUBLIC
  ${CMAKE_SOURCE_DIR}/Libraries/Rdk-HardwareLib/GUI/Qt
  ${CMAKE_SOURCE_DIR}/Libraries/Rdk-HardwareLib/GUI/Qt/widgets
  ${CMAKE_SOURCE_DIR}/Rdk/GUI/Qt
  ${CMAKE_SOURCE_DIR}/Libraries/Rdk-HardwareLib/Core
)

find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Svg QUIET)
target_link_libraries(${GUI_TARGET_NAME} PUBLIC
  ${TARGET_NAME}
  rdk.static.qt
  Nmsdk::Qt::Widgets
)
if(TARGET Qt${QT_VERSION_MAJOR}::Svg)
  target_link_libraries(${GUI_TARGET_NAME} PUBLIC Qt${QT_VERSION_MAJOR}::Svg)
else()
  message(WARNING "Qt Svg not found; HardwareLib GUI diagram requires Qt Svg")
endif()

target_compile_definitions(${GUI_TARGET_NAME} PUBLIC RDK_QT)

if(MSVC)
  target_compile_options(${GUI_TARGET_NAME} PRIVATE /utf-8)
endif()
