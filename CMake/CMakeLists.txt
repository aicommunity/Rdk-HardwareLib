cmake_minimum_required(VERSION 3.16)
project(Rdk_HardwareLib LANGUAGES CXX)

set(TARGET_NAME Rdk-HardwareLib.qt)

add_library(${TARGET_NAME} STATIC)

target_include_directories(${TARGET_NAME}
  PUBLIC
    ${CMAKE_SOURCE_DIR}/Rdk/Deploy/Include
    ${CMAKE_SOURCE_DIR}/Libraries/Rdk-HardwareLib/Core
    ${CMAKE_SOURCE_DIR}/Rdk/Core/Engine
    ${CMAKE_SOURCE_DIR}/Rdk/Core/System
)

file(GLOB HW_SOURCES
  ${CMAKE_SOURCE_DIR}/Libraries/Rdk-HardwareLib/Core/*.cpp
)

target_sources(${TARGET_NAME} PRIVATE ${HW_SOURCES})

target_compile_definitions(${TARGET_NAME} PUBLIC RDK_QT RDK_LIB_CVS_REVISION=0)

find_package(Qt5 REQUIRED COMPONENTS Core SerialPort)
target_link_libraries(${TARGET_NAME} PUBLIC Qt5::Core Qt5::SerialPort Threads::Threads rdk.static.qt Rdk-BasicLib.qt)






