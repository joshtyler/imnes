cmake_minimum_required(VERSION 3.16)
project(nesemu)

set(CMAKE_CXX_STANDARD 20)

add_executable(nesemu main.cpp Cpu6502.cpp Cpu6502.h Cpu6502_instructions.h)