cmake_minimum_required(VERSION 3.1)
set(CMAKE_CXX_STANDARD 11)
project(Car)

include_directories(${PROJECT_SOURCE_DIR}/inc
    ${PROJECT_SOURCE_DIR}/src)

add_executable(Car src/main.cpp src/Logger.cpp src/KillFlag.cpp src/Signal.cpp)


