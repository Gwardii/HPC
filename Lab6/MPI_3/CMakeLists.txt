cmake_minimum_required(VERSION 3.16)
project(cpp_generic)

find_package(MPI REQUIRED)
find_package(benchmark REQUIRED)

add_executable(advection_explicit_mpi advection_explicit_mpi.cpp)
set_target_properties(advection_explicit_mpi PROPERTIES CXX_STANDARD 20)
target_link_libraries(advection_explicit_mpi PRIVATE MPI::MPI_CXX benchmark::benchmark)
