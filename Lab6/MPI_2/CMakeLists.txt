cmake_minimum_required(VERSION 3.16)
project(cpp_generic)

find_package(MPI REQUIRED)
find_package(benchmark REQUIRED)

add_executable(serial_impl serial_impl.cpp)
set_target_properties(serial_impl PROPERTIES CXX_STANDARD 20)

add_executable(mpi_impl mpi_impl.cpp)
set_target_properties(mpi_impl PROPERTIES CXX_STANDARD 20)
target_link_libraries(mpi_impl PRIVATE MPI::MPI_CXX)

add_executable(serial_benchmark serial_benchmark.cpp)
set_target_properties(serial_benchmark PROPERTIES CXX_STANDARD 20)
target_link_libraries(serial_benchmark PRIVATE MPI::MPI_CXX benchmark::benchmark)

add_executable(mpi_benchmark mpi_benchmark.cpp)
set_target_properties(mpi_benchmark PROPERTIES CXX_STANDARD 20)
target_link_libraries(mpi_benchmark PRIVATE MPI::MPI_CXX benchmark::benchmark)