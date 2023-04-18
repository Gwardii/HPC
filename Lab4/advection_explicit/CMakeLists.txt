cmake_minimum_required(VERSION 3.16)
project(cpp_generic)

find_package(TBB REQUIRED)
find_package(benchmark REQUIRED)

add_executable(advection_serial advection_serial.cpp)

