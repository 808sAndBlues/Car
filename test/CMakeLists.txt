cmake_minimum_required(VERSION 3.1)
set(CMAKE_CXX_STANDARD 11)
project(Car)

enable_testing()

add_executable(ut test.cpp)

target_link_libraries(ut gtest gtest_main pthread)
