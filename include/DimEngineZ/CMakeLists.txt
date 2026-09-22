cmake_minimum_required(VERSION 3.15)
project(DimEngineZ)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(raylib REQUIRED)

add_library(DimEngineZ
    src/common.cpp
    src/physics.cpp
    src/manager.cpp
    src/input.cpp
    src/logger.cpp
)

target_include_directories(DimEngineZ PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(DimEngineZ PRIVATE raylib)