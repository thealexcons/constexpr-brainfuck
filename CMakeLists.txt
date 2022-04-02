cmake_minimum_required(VERSION 3.21)
project(compile_time_code_generator)

set(CMAKE_CXX_STANDARD 17)

add_executable(compile_time_code_generator main.cpp brainfuck.h)
