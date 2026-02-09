cmake_minimum_required(VERSION 3.20)

# Project name and language
project(ABSolver LANGUAGES CXX)

# Specify C++ standard
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Gather all source files (non-recursive)
file(GLOB SOURCES "src/*.cpp")

# Create executable
add_executable(AB ${SOURCES})

# Include directories for this target
target_include_directories(AB PRIVATE include)

# Set the output directory to be the build folder
set_target_properties(AB PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}"
    RUNTIME_OUTPUT_DIRECTORY_DEBUG "${CMAKE_BINARY_DIR}"
    RUNTIME_OUTPUT_DIRECTORY_RELEASE "${CMAKE_BINARY_DIR}"
)
