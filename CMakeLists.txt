cmake_minimum_required(VERSION 3.7)

project(chip8)

find_package(SDL2 REQUIRED)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS ON)

set(SOURCES src/main.cpp src/cpu.cpp src/screenView.cpp)

add_executable(chip8 ${SOURCES})
target_link_libraries(chip8 PRIVATE SDL2::SDL2)
