cmake_minimum_required(VERSION 3.5)
project(lecture2)

find_package(benchmark REQUIRED)

macro(register_example name)
    add_executable(${name} ${name}.cpp)
    set_target_properties(${name} PROPERTIES CXX_STANDARD 20)
    target_link_libraries(${name} PRIVATE benchmark::benchmark)
endmacro()

register_example(01a_vec_mult2)
register_example(01b_vec_quad)
register_example(01c_vec_quad_unaligned)
register_example(01d_vec_quad_aligned)
register_example(02a_pure)
register_example(02b_intrin)
register_example(02c_intrin_optimized)
