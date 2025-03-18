cmake_minimum_required(VERSION 3.16)
project(WakeOMatic)

# Use C++20 for compatibility
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Fix MSVC build flags
if (MSVC)
    add_compile_options(/std:c++20 /EHsc)  # Use C++20, enable exceptions
else()
    add_compile_options(-std=c++20 -pthread)  # Remove `-lstdc++`
endif()

# Find required packages
find_package(OpenCV REQUIRED)
find_package(Boost REQUIRED COMPONENTS log log_setup filesystem system thread)
find_package(OpenMP REQUIRED)

# Include directories
include_directories(${OpenCV_INCLUDE_DIRS} ${Boost_INCLUDE_DIRS} ${CMAKE_SOURCE_DIR}/src/modules)

# Define test source files
set(TEST_SOURCE_FILES 
    test/src/tests.h 
    test/src/tests.cpp 
    test/src/cppTests.h 
    test/src/cppTests.cpp
)

# Define test executable source file
set(TEST_EXECUTABLE test/src/runTests.cpp)

# Create a STATIC test library
add_library(wake-o-matic-tests STATIC ${TEST_SOURCE_FILES})

# Create the test executable
add_executable(wake-o-matic-tester ${TEST_EXECUTABLE})

# Ensure `wake-o-matic-tests` links to `wake-o-matic-modules`
target_link_libraries(wake-o-matic-tests LINK_PUBLIC wake-o-matic-modules)

# Link the test executable with necessary libraries
target_link_libraries(wake-o-matic-tester LINK_PUBLIC wake-o-matic-tests ${Boost_LIBRARIES} ${OpenCV_LIBS} OpenMP::OpenMP_CXX)

# Define Unit Tests
add_test(NAME unitTests COMMAND wake-o-matic-tester)
