cmake_minimum_required (VERSION 3.16...3.27)
project (eshell
         LANGUAGES CXX C)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(SOURCES
    src/parser.c
    src/main.cpp
)

add_executable (${PROJECT_NAME} ${SOURCES})
target_include_directories(${PROJECT_NAME} PRIVATE include)

target_compile_options(${PROJECT_NAME} PRIVATE
                       -Weverything -Wno-c++98-compat -Wno-pre-c++17-compat -Wno-shadow-field-in-constructor -march=native) #-fsanitize=address,undefined)
#target_link_options(${PROJECT_NAME} PRIVATE -fsanitize=address,undefined)
