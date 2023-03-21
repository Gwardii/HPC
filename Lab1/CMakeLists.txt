cmake_minimum_required(VERSION 3.5)
project(my_project_name) # this does not matter for our simple case
add_executable(bench_demo src/BenchmarkDemo.cpp) # this determines the executable name
target_include_directories(bench_demo PUBLIC include)
target_compile_features(bench_demo PUBLIC cxx_std_20) # we need C++20
find_package(benchmark REQUIRED)
target_link_libraries(bench_demo PUBLIC benchmark::benchmark)