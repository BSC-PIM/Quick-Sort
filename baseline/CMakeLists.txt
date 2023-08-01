cmake_minimum_required(VERSION 3.26)
project(baseline C)

set(CMAKE_C_STANDARD 11)

add_executable(baseline main.c
        quicksort.c
        quicksort.h)
