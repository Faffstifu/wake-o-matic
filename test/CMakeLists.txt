cmake_minimum_required(VERSION 3.16)
project(Wake-o-matic-tests)

# Enable testing
include(CTest)
enable_testing()

# ✅ Prevent duplicate target error by checking if the target exists
if (NOT TARGET wake-o-matic-tests)
    set(TEST_SOURCE_FILES 
        ${CMAKE_SOURCE_DIR}/test/src/tests.h 
        ${CMAKE_SOURCE_DIR}/test/src/tests.cpp 
        ${CMAKE_SOURCE_DIR}/test/src/cppTests.h 
        ${CMAKE_SOURCE_DIR}/test/src/cppTests.cpp
    )

    add_library(wake-o-matic-tests STATIC ${TEST_SOURCE_FILES})
endif()

# ✅ Prevent duplicate target error for executable
if (NOT TARGET wake-o-matic-tester)
    set(TEST_EXECUTABLE ${CMAKE_SOURCE_DIR}/test/src/runTests.cpp)
    add_executable(wake-o-matic-tester ${TEST_EXECUTABLE})

    target_link_libraries(wake-o-matic-tester LINK_PUBLIC 
        wake-o-matic-tests 
        wake-o-matic-modules 
        ${Boost_LIBRARIES} 
        ${OpenCV_LIBS}
    )

    add_test(NAME unitTests COMMAND wake-o-matic-tester)
endif()
