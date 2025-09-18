cmake_minimum_required(VERSION 3.1)
set(CMAKE_CXX_STANDARD 11)
project(Car)

enable_testing()

get_filename_component(PARENT_DIR ../ ABSOLUTE)

include_directories(${PARENT_DIR}/inc)

set(SRC ${PARENT_DIR}/src)

add_executable(ut QueueTest.cpp ${SRC}/Queue.cpp)

target_link_libraries(ut gtest gtest_main pthread)
