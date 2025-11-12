# Rdk-HardwareLib Integration Tests CMake Configuration
cmake_minimum_required(VERSION 3.16)

# Find Google Test
find_package(GTest REQUIRED)

# Create test executable for HardwareLib integration tests
add_executable(RdkHardwareLibIntegrationTests
    TestHardwareLibIntegration.cpp
    ${CMAKE_SOURCE_DIR}/Libraries/Libraries_con.cpp
)

target_include_directories(RdkHardwareLibIntegrationTests PRIVATE
    ${CMAKE_SOURCE_DIR}/Rdk/Core
    ${CMAKE_SOURCE_DIR}/Rdk/Deploy/Include
    ${CMAKE_SOURCE_DIR}/Libraries/Rdk-HardwareLib/Core
    ${CMAKE_SOURCE_DIR}/Libraries
    ${CMAKE_SOURCE_DIR}/Tests
)

target_link_libraries(RdkHardwareLibIntegrationTests PRIVATE
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

target_compile_features(RdkHardwareLibIntegrationTests PRIVATE cxx_std_20)
target_compile_definitions(RdkHardwareLibIntegrationTests PRIVATE RDK_QT RDK_LIB_CVS_REVISION=0)

# Register tests
add_test(NAME RdkHardwareLibIntegrationTests COMMAND RdkHardwareLibIntegrationTests)
