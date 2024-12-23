# Project name
project(kmeans)

# C++ Standard
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_CXX_FLAGS "-Wall -O2 -pthread")



# Include directories
include_directories(${PROJECT_SOURCE_DIR})

# Source files
set(SOURCES main.cpp kmeans.cpp dataGenerator.cpp tests.cpp)

# Executable target
add_executable(kmeans ${SOURCES})


find_package(Threads REQUIRED)
target_link_libraries(kmeans Threads::Threads)