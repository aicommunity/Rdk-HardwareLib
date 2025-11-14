# Rdk-HardwareLib Unit Tests CMake Configuration
cmake_minimum_required(VERSION 3.16)

# Find Google Test
find_package(GTest REQUIRED)

# Create test executable for HardwareLib tests
add_executable(RdkHardwareLibTests
    TestHardwareLibComponents.cpp
    TestHardwareLibRegistration.cpp
    ${CMAKE_SOURCE_DIR}/Libraries/Libraries_con.cpp
)

target_include_directories(RdkHardwareLibTests PRIVATE
    ${CMAKE_SOURCE_DIR}/Rdk/Core
    ${CMAKE_SOURCE_DIR}/Rdk/Deploy/Include
    ${CMAKE_SOURCE_DIR}/Libraries/Rdk-HardwareLib/Core
    ${CMAKE_SOURCE_DIR}/Libraries
    ${CMAKE_SOURCE_DIR}/Tests
)

target_link_libraries(RdkHardwareLibTests PRIVATE
    rdk.static.qt
    Rdk-HardwareLib.qt
    Rdk-CvBasicLib.qt
    Nmsdk-PulseLib.qt
    Nmsdk-MotionControlLib.qt
    TestHelpers
    GTest::gtest
    GTest::gtest_main
    glog::glog
    Threads::Threads
)

target_compile_features(RdkHardwareLibTests PRIVATE cxx_std_20)
target_compile_definitions(RdkHardwareLibTests PRIVATE RDK_QT RDK_LIB_CVS_REVISION=0)

# Register tests
add_test(NAME RdkHardwareLibTests COMMAND RdkHardwareLibTests)
