cmake_minimum_required(VERSION 3.10)
project(file_system_demo C)

set(CMAKE_C_STANDARD 11)


add_executable(file_system_demo main.c filesystemlib.c)