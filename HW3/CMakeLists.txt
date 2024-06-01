cmake_minimum_required (VERSION 3.22...3.27)
project (recext2fs
         LANGUAGES CXX C)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(SOURCES
    src/ext2fs_print.c
    src/identifier.cpp
    src/main.cpp
    src/recext2fs.cpp
)

add_executable (${PROJECT_NAME} ${SOURCES})
target_include_directories(${PROJECT_NAME} PRIVATE include)
