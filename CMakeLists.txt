cmake_minimum_required(VERSION 3.21)
project(halado_cpp_bead)

set(CMAKE_CXX_STANDARD 17)
set(GCC_COVERAGE_COMPILE_FLAGS "-Wall -Wextra -Werror -pedantic -fsanitize=address,leak,undefined")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${GCC_COVERAGE_COMPILE_FLAGS}" )

add_executable(halado_cpp_bead main.cpp id_bimap.hpp avl_tree.hpp)
