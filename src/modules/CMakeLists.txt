cmake_minimum_required(VERSION 3.16)

set(MODULE_SOURCE_FILES logging.h logging.cpp actionStateMachine.cpp camera.cpp eyeStatus.cpp frameProcessor.cpp sleepDetect.cpp)

add_library (wake-o-matic-modules ${MODULE_SOURCE_FILES})

target_include_directories (wake-o-matic-modules PUBLIC ${CUSTOM_LINK_LIBRARIES})