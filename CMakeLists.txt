cmake_minimum_required(VERSION 3.22)

set(TARGET ForetellVisualBugFixMod)
project(${TARGET})

set(UE4SS_PATH "$ENV{UE4SS_PATH}" CACHE PATH "Path to UE4SS project directory")
if(UE4SS_PATH STREQUAL "")
    message(FATAL_ERROR "UE4SS_PATH is not set!")
endif()

message(STATUS "UE4SS_PATH = ${UE4SS_PATH}")

if (NOT IS_DIRECTORY "${UE4SS_PATH}")
    message(FATAL_ERROR "UE4SS_PATH is not a directory!")
endif()
if (NOT EXISTS "${UE4SS_PATH}/CMakeLists.txt")
    message(FATAL_ERROR "UE4SS_PATH is not valid!")
endif()

add_subdirectory(${UE4SS_PATH} "${CMAKE_CURRENT_BINARY_DIR}/RE-UE4SS")

add_library(${TARGET} SHARED
    "src/dllmain.cpp"
    "src/ForetellVisualBugFixMod.cpp"
)

target_include_directories(${TARGET} PRIVATE src)
target_link_libraries(${TARGET} PUBLIC UE4SS)