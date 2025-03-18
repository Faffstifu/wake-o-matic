cmake_minimum_required(VERSION 3.16)
project(WakeOMatic)

# ✅ Fix Boost policy warning
cmake_policy(SET CMP0167 NEW)

# ✅ Use C++20 for compatibility
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# ✅ Global Macro Definitions to Prevent Windows API Conflicts
add_definitions(-DWIN32_LEAN_AND_MEAN -DNOMINMAX -DNOGDI -DNOUSER -DNOSERVICE)  

# ✅ Fix MSVC build flags
if (MSVC)
    add_compile_options(/std:c++20 /EHsc /W3 /MP)  # `/MP` speeds up compilation
else()
    add_compile_options(-std=c++20)  # ✅ Keep standard C++20 for GCC/Clang
endif()

# ✅ Ensure OpenCV is found correctly
set(CMAKE_PREFIX_PATH "C:/opencv/build")

find_package(OpenCV REQUIRED QUIET)
if(OpenCV_FOUND)
    message(STATUS "✅ OpenCV found: ${OpenCV_INCLUDE_DIRS}")
    include_directories(${OpenCV_INCLUDE_DIRS})
    link_directories(${OpenCV_LIBRARY_DIRS})
else()
    message(FATAL_ERROR "❌ OpenCV NOT found! Check installation path.")
endif()

# ✅ Fix OpenCV conflicting macro issue (Ensure this is defined BEFORE including OpenCV)
add_definitions(-DOPENCV_DISABLE_THREAD_SUPPORT)

# ✅ Explicitly Set Boost Paths
set(BOOST_ROOT "C:/boost")  # Adjust this if Boost is installed elsewhere
set(BOOST_INCLUDEDIR "C:/boost/include/boost-1_87")
set(BOOST_LIBRARYDIR "C:/boost/lib")

find_package(Boost REQUIRED COMPONENTS log log_setup filesystem system thread)
if(Boost_FOUND)
    message(STATUS "✅ Boost found at: ${Boost_INCLUDE_DIRS}")
    include_directories(${Boost_INCLUDE_DIRS})
    link_directories(${Boost_LIBRARY_DIR})
else()
    message(FATAL_ERROR "❌ Boost NOT found! Check installation path.")
endif()

# ✅ Find OpenMP (For Multi-threading)
find_package(OpenMP REQUIRED)
if (OpenMP_CXX_FOUND)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
endif()

# ✅ Define absolute paths for Haar cascades
add_definitions(-DFACE_CASCADE_PATH="${CMAKE_SOURCE_DIR}/src/data/haarcascade_frontalface_default.xml")
add_definitions(-DEYES_CASCADE_PATH="${CMAKE_SOURCE_DIR}/src/data/haarcascade_eye.xml")

# ✅ Define source files
set(MAIN_SOURCE_FILES ${CMAKE_SOURCE_DIR}/src/main.cpp)

set(MODULE_SOURCE_FILES 
    ${CMAKE_SOURCE_DIR}/src/modules/frameProcessor.cpp 
    ${CMAKE_SOURCE_DIR}/src/modules/eyeStatus.cpp 
    ${CMAKE_SOURCE_DIR}/src/modules/camera.cpp
    ${CMAKE_SOURCE_DIR}/src/modules/sleepDetect.cpp
    ${CMAKE_SOURCE_DIR}/src/modules/actionStateMachine.cpp   
    ${CMAKE_SOURCE_DIR}/src/modules/logging.cpp
)

# ✅ Link dependencies
set(CUSTOM_LINK_LIBRARIES ${OpenCV_LIBS} ${Boost_LIBRARIES} ${OpenMP_LIBS})

if (SAVE_LOG)
    set(CUSTOM_LINK_LIBRARIES ${CUSTOM_LINK_LIBRARIES} ${Boost_LOG_LIBRARY} ${Boost_LOG_SETUP_LIBRARY})
endif()

# ✅ Ensure the library is built first
add_library(wake-o-matic-modules STATIC ${MODULE_SOURCE_FILES})

# ✅ Ensure the compiler finds include files
target_include_directories(wake-o-matic-modules PUBLIC ${CMAKE_SOURCE_DIR}/src/modules)

# ✅ Link necessary libraries to modules
target_link_libraries(wake-o-matic-modules PUBLIC ${CUSTOM_LINK_LIBRARIES})

# ✅ Now create the main executable (AFTER the library is created)
add_executable(wake-o-matic-main ${MAIN_SOURCE_FILES})

# ✅ Set output directory for the executable
set_target_properties(wake-o-matic-main PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

# ✅ Ensure `wake-o-matic-main` links to the modules correctly
target_link_libraries(wake-o-matic-main PUBLIC wake-o-matic-modules ${CUSTOM_LINK_LIBRARIES})
