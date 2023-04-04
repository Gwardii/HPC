cmake_minimum_required(VERSION 3.5)
project(lab2_project)

find_package(benchmark REQUIRED)

add_executable(lab3 src/BenchmarkMain.cpp
        src/BenchmarkExample.cpp
        # src/Goto.cpp
        )
target_include_directories(lab3 PRIVATE include)
set_target_properties(lab3 PROPERTIES CXX_STANDARD 17)
target_link_libraries(lab3 PRIVATE benchmark::benchmark)
