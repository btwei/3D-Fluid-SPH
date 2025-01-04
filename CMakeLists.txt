# Minimum CMake version
cmake_minimum_required(VERSION 3.15)

# Project name and version
project(fluidSimulation VERSION 1.0)

# Set the C++ standard
set(CMAKE_CXX_STANDARD 17)

# Find required packages
find_package(glfw3 REQUIRED)
find_package(OpenGL REQUIRED)

# Get all sources files from the src directory
file(GLOB_RECURSE SOURCE_FILES ${CMAKE_SOURCE_DIR}/src/*.cpp)

# Add the executable
add_executable(fluidSimulation src/main.cpp src/FluidSim.cpp src/Renderer.cpp src/Window.cpp src/glad.c ${SOURCE_FILES})

# Include directories
target_include_directories(fluidSimulation PRIVATE ${CMAKE_SOURCE_DIR}/include)

# Link libraries
target_link_libraries(fluidSimulation glfw OpenGL)