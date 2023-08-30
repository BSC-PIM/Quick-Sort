cmake_minimum_required(VERSION 3.26)
project(DpuLevel C)

set(CMAKE_C_STANDARD 99)

set(CMAKE_C_FLAGS "-fopenmp -O3")


# PLAYGROUND EXECUTABLE
add_executable(DpuLevel main.c
        commons/dataStructure/work_queue.h
        commons/dataStructure/job.h
        commons/dataStructure/work_queue.c
        sorts/quicksort/quicksort_mt.c
        sorts/quicksort/quicksort_mt.h
        sorts/radix/msb_radix_parallel_in_place.c
        sorts/radix/msb_radix_sequential_in_place.c
        utils/utils.h
        commons/quicksort_common.c
        commons/quicksort_common.h
        commons/radixsort_common.c
        commons/radixsort_common.h
        sorts/radix/lsb_radix_sequential_extra_space.c
        sorts/radix/lsb_radix_sequential_extra_space.h
        sorts/radix/lsb_radix_sequential_extra_space.c
        dataDistribution/merge/partition_and_merge.c
        dataDistribution/merge/partition_and_merge.h
        dataDistribution/worker.h
        dataDistribution/host.h
        dataDistribution/sort_prop.h
        config.h
)


# unity test framework executable
add_executable(Test tests/work_queue_test.c
        commons/dataStructure/work_queue.h
        commons/dataStructure/job.h
        commons/dataStructure/work_queue.c
        ../../libs/Unity/src/unity.c
        utils/utils.h
)

#-------------------------------- BENCHMARKS --------------------------------#
#_________________________________ QUICKSORT ________________________________#
add_executable(threads_num_bench_qsort benchmarks/benches/sortBenches/quicksort/threads_num_bench.c
        benchmarks/commons/common.c
        benchmarks/commons/common.h
        commons/dataStructure/work_queue.h
        commons/dataStructure/job.h
        commons/dataStructure/work_queue.c
        sorts/quicksort/quicksort_mt.c
        sorts/quicksort/quicksort_mt.h
        utils/utils.h
        commons/quicksort_common.c
        commons/quicksort_common.h
        ../../libs/Flag-Parser/flag_parser.c
)


add_executable(min_chunk_size_bench_qsort benchmarks/benches/sortBenches/quicksort/min_chunk_size_bench.c
        benchmarks/commons/common.c
        benchmarks/commons/common.h
        commons/dataStructure/work_queue.h
        commons/dataStructure/job.h
        commons/dataStructure/work_queue.c
        sorts/quicksort/quicksort_mt.c
        sorts/quicksort/quicksort_mt.h
        utils/utils.h
        commons/quicksort_common.c
        commons/quicksort_common.h
        ../../libs/Flag-Parser/flag_parser.c
)


add_executable(buffer_size_bench_qsort benchmarks/benches/sortBenches/quicksort/buffer_size_bench.c
        benchmarks/commons/common.c
        benchmarks/commons/common.h
        commons/dataStructure/work_queue.h
        commons/dataStructure/job.h
        commons/dataStructure/work_queue.c
        sorts/quicksort/quicksort_mt.c
        sorts/quicksort/quicksort_mt.h
        utils/utils.h
        commons/quicksort_common.c
        commons/quicksort_common.h
        ../../libs/Flag-Parser/flag_parser.c
)
#_________________________________ RADIX SORT ________________________________#
add_executable(threads_num_bench_rsort benchmarks/benches/sortBenches/radixsort/threads_num_bench.c
        benchmarks/commons/common.c
        benchmarks/commons/common.h
        sorts/radix/msb_radix_parallel_in_place.c
        sorts/radix/msb_radix_sequential_in_place.c
        utils/utils.h
        commons/radixsort_common.c
        commons/radixsort_common.h
        ../../libs/Flag-Parser/flag_parser.c
)

add_executable(min_chunk_size_bench_rsort benchmarks/benches/sortBenches/radixsort/min_chunk_size_bench.c
        benchmarks/commons/common.c
        benchmarks/commons/common.h
        sorts/radix/msb_radix_parallel_in_place.c
        sorts/radix/msb_radix_sequential_in_place.c
        utils/utils.h
        commons/radixsort_common.c
        commons/radixsort_common.h
        ../../libs/Flag-Parser/flag_parser.c
)

add_executable(buffer_size_bench_rsort benchmarks/benches/sortBenches/radixsort/buffer_size_bench.c
        benchmarks/commons/common.c
        benchmarks/commons/common.h
        sorts/radix/msb_radix_parallel_in_place.c
        sorts/radix/msb_radix_sequential_in_place.c
        utils/utils.h
        commons/radixsort_common.c
        commons/radixsort_common.h
        ../../libs/Flag-Parser/flag_parser.c

)


#________________________________ Data Distribution ____________________________#
#________________________________ Merge Partition ______________________________#
add_executable(merge_partition_bench benchmarks/benches/distributionBenches/merge_partition_bench.c
        benchmarks/commons/common.c
        benchmarks/commons/common.h
        commons/dataStructure/work_queue.h
        commons/dataStructure/job.h
        commons/dataStructure/work_queue.c
        sorts/quicksort/quicksort_mt.c
        sorts/quicksort/quicksort_mt.h
        dataDistribution/worker.h
        dataDistribution/host.h
        dataDistribution/sort_prop.h
        dataDistribution/merge/partition_and_merge.c
        dataDistribution/merge/partition_and_merge.h
        utils/utils.h
        commons/quicksort_common.c
        commons/quicksort_common.h
        ../../libs/Flag-Parser/flag_parser.c
)

include_directories("." "utils" "commons/dataStructure" "commons" "../../libs/Flag-Parser")
target_include_directories(Test PRIVATE "../../libs/Unity/src")

target_include_directories(threads_num_bench_qsort PRIVATE "./" "benchmarks/commons" "./sorts/quicksort")
target_include_directories(min_chunk_size_bench_qsort PRIVATE "./" "benchmarks/commons" "./sorts/quicksort")
target_include_directories(buffer_size_bench_qsort PRIVATE "./" "benchmarks/commons" "./sorts/quicksort")

target_include_directories(threads_num_bench_rsort PRIVATE "./" "benchmarks/commons" "./sorts/radix")
target_include_directories(min_chunk_size_bench_rsort PRIVATE "./" "benchmarks/commons" "./sorts/radix")
target_include_directories(buffer_size_bench_rsort PRIVATE "./" "benchmarks/commons" "./sorts/radix")

target_include_directories(merge_partition_bench PRIVATE "./" "benchmarks/commons" "./sorts/quicksort" "./dataDistribution")
