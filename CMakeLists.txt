cmake_minimum_required(VERSION 3.1)
set(CMAKE_CXX_STANDARD 11)
project(Car)

include_directories(${PROJECT_SOURCE_DIR}/inc)

add_executable(Car 
    src/main.cpp src/Logger.cpp src/KillFlag.cpp 
    src/Signal.cpp src/Queue.cpp
    src/Epoll.cpp)

enable_testing()
add_subdirectory(test)

target_link_libraries(Car bcm2835)

