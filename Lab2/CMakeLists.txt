cmake_minimum_required(VERSION 3.5)
project(lab2_project)

find_package(benchmark REQUIRED)

add_executable(lab2 src/BenchmarkMain.cpp
        src/BenchmarkExample.cpp
        src/Goto.cpp
        )
target_include_directories(lab2 PRIVATE include)
set_target_properties(lab2 PROPERTIES CXX_STANDARD 20)
target_link_libraries(lab2 PRIVATE benchmark::benchmark)
