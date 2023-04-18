cmake_minimum_required(VERSION 3.16)
project(cpp_generic)

find_package(TBB REQUIRED)
find_package(benchmark REQUIRED)

add_executable(serial serial.cpp)
set_target_properties(serial PROPERTIES CXX_STANDARD 20)
target_link_libraries(serial PRIVATE benchmark::benchmark)

add_executable(tbb_parallel_for tbb_parallel_for.cpp)
set_target_properties(tbb_parallel_for PROPERTIES CXX_STANDARD 20)
target_link_libraries(tbb_parallel_for PRIVATE TBB::tbb benchmark::benchmark)

add_executable(tbb_false_sharing tbb_false_sharing.cpp)
set_target_properties(tbb_false_sharing PROPERTIES CXX_STANDARD 20)
target_link_libraries(tbb_false_sharing PRIVATE TBB::tbb benchmark::benchmark)

add_executable(tbb_parallel_reduce tbb_parallel_reduce.cpp)
set_target_properties(tbb_parallel_reduce PROPERTIES CXX_STANDARD 20)
target_link_libraries(tbb_parallel_reduce PRIVATE TBB::tbb benchmark::benchmark)

