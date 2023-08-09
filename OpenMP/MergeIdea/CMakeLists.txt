cmake_minimum_required(VERSION 3.26)
project(MergeIdea C)

set(CMAKE_C_STANDARD 99)

set(CMAKE_C_FLAGS "-fopenmp")


add_executable(MergeIdea main.c
        utils/work_queue.h
        utils/job.h
        utils/work_queue.c
)


add_executable(Test tests/work_queue_test.c
        utils/work_queue.h
        utils/job.h
        utils/work_queue.c
        ../../libs/Unity/src/unity.c
)

include_directories("utils")
target_include_directories(Test PRIVATE "../../libs/Unity/src")
