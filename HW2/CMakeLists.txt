cmake_minimum_required (VERSION 3.22...3.27)
project (simulator
         LANGUAGES CXX C)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(SOURCES
    src/helper.c
    src/WriteOutput.c
    src/main.cpp
    src/simulator.cpp
    src/narrow_bridge.cpp
    src/ferry.cpp
    src/crossroad.cpp
    src/car.cpp
)

add_executable (${PROJECT_NAME} ${SOURCES})
target_include_directories(${PROJECT_NAME} PRIVATE include)

set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)
target_link_libraries(${PROJECT_NAME} PRIVATE Threads::Threads)
