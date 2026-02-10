cmake_minimum_required(VERSION 3.18)

# Make sure we are on Windows
if(NOT WIN32)
    message(FATAL_ERROR "Smash Soda can only be built on Windows.")
endif()

project(SmashSoda LANGUAGES CXX)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Force C++14
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Detect architecture for output directories
if(NOT CMAKE_VS_PLATFORM_NAME)
    if(CMAKE_SIZEOF_VOID_P EQUAL 8)
        set(ARCH "x64")
    else()
        set(ARCH "x86")
    endif()
else()
    set(ARCH "${CMAKE_VS_PLATFORM_NAME}")
endif()

# Output folders
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/${ARCH}/lib")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/${ARCH}/lib")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG "${CMAKE_SOURCE_DIR}/${ARCH}/debug")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE "${CMAKE_SOURCE_DIR}/${ARCH}/release")

# Add dependencies if they have their own CMakeLists
if(EXISTS "${CMAKE_SOURCE_DIR}/dependencies/CMakeLists.txt")
    add_subdirectory(dependencies)
endif()

# Add main project
add_subdirectory(SmashSoda)