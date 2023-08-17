cmake_minimum_required(VERSION 3.26)
project(DpuLevel C)

set(CMAKE_C_STANDARD 99)

set(CMAKE_C_FLAGS "-fopenmp")


add_executable(DpuLevel main.c
        commons/dataStructure/work_queue.h
        commons/dataStructure/job.h
        commons/dataStructure/work_queue.c
        quicksort_mt.c
        quicksort_mt.h
        utils/utils.h
        commons/quicksort_common.c
        commons/quicksort_common.h
)


add_executable(Test tests/work_queue_test.c
        commons/dataStructure/work_queue.h
        commons/dataStructure/job.h
        commons/dataStructure/work_queue.c
        ../../libs/Unity/src/unity.c
        utils/utils.h
)


add_executable(threads_num_bench benchmarks/threads_num_bench.c
        benchmarks/commons/common.c
        benchmarks/commons/common.h
        commons/dataStructure/work_queue.h
        commons/dataStructure/job.h
        commons/dataStructure/work_queue.c
        quicksort_mt.c
        quicksort_mt.h
        utils/utils.h
        commons/quicksort_common.c
        commons/quicksort_common.h
        ../../libs/Flag-Parser/flag_parser.c
)


add_executable(min_chunk_size_bench benchmarks/min_chunk_size_bench.c
        benchmarks/commons/common.c
        benchmarks/commons/common.h
        commons/dataStructure/work_queue.h
        commons/dataStructure/job.h
        commons/dataStructure/work_queue.c
        quicksort_mt.c
        quicksort_mt.h
        utils/utils.h
        commons/quicksort_common.c
        commons/quicksort_common.h
        ../../libs/Flag-Parser/flag_parser.c
)



add_executable(buffer_size_bench benchmarks/buffer_size_bench.c
        benchmarks/commons/common.c
        benchmarks/commons/common.h
        commons/dataStructure/work_queue.h
        commons/dataStructure/job.h
        commons/dataStructure/work_queue.c
        quicksort_mt.c
        quicksort_mt.h
        utils/utils.h
        commons/quicksort_common.c
        commons/quicksort_common.h
        ../../libs/Flag-Parser/flag_parser.c
)

include_directories("utils" "commons/dataStructure" "commons" "../../libs/Flag-Parser")
target_include_directories(Test PRIVATE "../../libs/Unity/src")
target_include_directories(threads_num_bench PRIVATE "./" "benchmarks/commons")
target_include_directories(min_chunk_size_bench PRIVATE "./" "benchmarks/commons")
target_include_directories(buffer_size_bench PRIVATE "./" "benchmarks/commons")

