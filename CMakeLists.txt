cmake_minimum_required(VERSION 3.16)
project(DimOrbit)
option(BUILD_TESTS "Build DimOrbit tests." OFF)


set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_library(DimOrbit 
    src/common.cpp
    src/clock.cpp
    src/spacecraft.cpp
    src/maneuvers.cpp
    src/controller.cpp
)
target_include_directories(DimOrbit PUBLIC ${CMAKE_SOURCE_DIR}/include)
add_subdirectory(${CMAKE_SOURCE_DIR}/include/DimEngineZ)

find_package(raylib REQUIRED)
target_link_libraries(DimOrbit
    PUBLIC
        raylib
        DimEngineZ
)

if (BUILD_TESTS)
    add_executable(DimOrbit_test tests/main.cpp
        src/renderer.cpp)
    target_link_libraries(DimOrbit_test PRIVATE DimOrbit)
endif()