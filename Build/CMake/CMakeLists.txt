cmake_minimum_required(VERSION 3.0)
project(Rdk-HardwareLib)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

find_package(QT NAMES Qt5 REQUIRED COMPONENTS Widgets)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Core Network Sql Xml SerialPort)

#set(QT_NO_GUI ON)

add_library(Rdk-HardwareLib STATIC)

add_definitions(-DLIBRDK_LIBRARY_EXPORT -DRDK_UNICODE_RUN -DRDK_QT)

set(VERSION 0)
add_definitions(-DRDK_LIB_CVS_REVISION=${VERSION})

include_directories(${CMAKE_SOURCE_DIR}/../../../../Rdk/Deploy/Include)

set(SOURCES
    ${CMAKE_SOURCE_DIR}/../../Core/UAdcSensor.cpp
    ${CMAKE_SOURCE_DIR}/../../Core/UArduinoConnect.cpp
    ${CMAKE_SOURCE_DIR}/../../Core/UArduinoSensor.cpp
    ${CMAKE_SOURCE_DIR}/../../Core/UHardwareLibrary.cpp
)

set(HEADERS
    ${CMAKE_SOURCE_DIR}/../../Core/UAdcSensor.h
    ${CMAKE_SOURCE_DIR}/../../Core/UArduinoConnect.h
    ${CMAKE_SOURCE_DIR}/../../Core/UArduinoSensor.h
    ${CMAKE_SOURCE_DIR}/../../Core/UHardwareLibrary.h
)

target_sources(Rdk-HardwareLib PRIVATE)

if(UNIX)
    set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/../../../../Bin/Platform/Linux/Lib.Qt)
elseif(WIN32)
    set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/../../../../Bin/Platform/Win/Lib.Qt)
endif()
