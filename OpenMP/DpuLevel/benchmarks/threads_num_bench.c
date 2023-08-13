#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>

#include "quicksort_mt.h"
#include "common.h"
#include "flag_parser.h"

#define THREADS_SIZE 6
#define ARRAY_SIZE_FLAG 10000
#define TEST_COUNT 100

void populate_wrapper(uint64_t *arr, size_t size) {
    POPULATE_ARR(arr, size, INTMAX_MAX);
}

bool verify_wrapper(uint64_t *arr, size_t size) {
    return verify(arr, size);
}

int main(int argc, char *argv[]) {
    int threads;
    size_t array_size;

    // Define the flags
    struct Flag flags[] = {
            {.name = "threads", .has_arg=required_argument, .val='t', .save=&threads, .type = INT16_T},
            {.name = "array-size", .has_arg=required_argument, .val='a', .save=&array_size, .type= INT64_T}
    };


    const char *Usage = "Usage: program_name [OPTIONS]\n\n"
                        "Description:\n"
                        "  This program demonstrates flag parsing using the flag_parser library.\n\n"
                        "Options:\n"
                        "--threads NUM                         Number of threads to use (required, range: 0 to 32,767)\n"
                        "--array-size NUM                      Size of the array (required, range: 0 to 32,767)\n"
                        "--help                                Display this help message and exit\n\n"
                        "Usage Examples:\n"
                        "  program_name --threads 4 --array-size 100   # Run with 4 threads and array size 100\n";


    // Parse the flags
    parse_flags(argc, argv, flags, sizeof(flags) / sizeof(flags[0]), Usage);


    uint64_t array[array_size];

    // set up the threads
    omp_set_num_threads(threads);
    GROUP_MIN_DIST = 128;

    // create a pointer function which is used for setup (get array pointer and array size then call POPULATE_ARR on it)
    void (*setup)(uint64_t *, size_t) = &populate_wrapper;
    bool (*verify)(uint64_t *, size_t) = &verify_wrapper;

    // benchmark the quicksort_task_parallelism
    bench_function(quicksort_baseline, array, array_size, TEST_COUNT, HOARE, setup, verify);
    bench_function(quicksort_task_parallelism, array, array_size, TEST_COUNT, HOARE, setup, verify);
    bench_function(quicksort_threadpool_parallelism, array, array_size, TEST_COUNT, HOARE, setup, verify);
    printf("\n");
}

//TODO CHECK STD IMPLEMENTATION