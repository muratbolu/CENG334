cmake_minimum_required (VERSION 3.22...3.27)
project (eshell
         LANGUAGES CXX C)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(SOURCES
    src/parser.c
    src/main.cpp
    src/eshell.cpp
)

add_executable (${PROJECT_NAME} ${SOURCES})
target_include_directories(${PROJECT_NAME} PRIVATE include)

target_compile_options(${PROJECT_NAME} PRIVATE
                       -Wall -Wextra -Wpedantic -march=native)
