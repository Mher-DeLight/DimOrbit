cmake_minimum_required(VERSION 3.16)
project(DimOrbit)
option(BUILD_TESTS "Build DimOrbit tests." OFF)


set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_library(DimOrbit src/DimOrbit.cpp)
target_include_directories(DimOrbit PUBLIC ${CMAKE_SOURCE_DIR}/include)
target_link_libraries(DimOrbit PRIVATE ${CMAKE_SOURCE_DIR}/include/DimEngineZ/src/DimEngineZ.a)
target_include_directories(DimOrbit PUBLIC ${CMAKE_SOURCE_DIR}/include/DimEngineZ/include)

find_package(raylib REQUIRED)
target_link_libraries(DimOrbit
    PUBLIC
        raylib
)

if (BUILD_TESTS)
    add_executable(DimOrbit_test tests/main.cpp)
    target_link_libraries(DimOrbit_test PRIVATE DimOrbit)
endif()