cmake_minimum_required(VERSION 3.16)
project(arrows C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS ON)

add_compile_options(-O3 -march=native -ffast-math -Wall -g -fopenmp)

include_directories(${CMAKE_SOURCE_DIR}/lib/include)
include_directories(${CMAKE_SOURCE_DIR}/include)

link_directories(${CMAKE_SOURCE_DIR}/lib)

file(GLOB_RECURSE SRC_FILES CONFIGURE_DEPENDS
        ${CMAKE_SOURCE_DIR}/src/*.c
)

add_executable(arrows ${SRC_FILES})

target_link_libraries(arrows
        raylib
        opengl32
        gdi32
        winmm
)

find_package(OpenMP REQUIRED)
if(OpenMP_C_FOUND)
    target_link_libraries(arrows OpenMP::OpenMP_C)
endif()
