#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>

#include "quicksort_mt.h"
#include "common.h"
#include "flag_parser.h"

#define THREADS_SIZE 6
#define ARRAY_SIZE_FLAG 10000

void populate_wrapper(uint64_t *arr, size_t size) {
    POPULATE_ARR(arr, size, 100);
}

bool verify_wrapper(uint64_t *arr, size_t size) {
    return verify(arr, size);
}

int main(int argc, char *argv[]) {
    int threads;
    int array_size;

    // Define the flags
    struct Flag flags[] = {
            {"threads",    required_argument, &threads,    NUMBER, THREADS_SIZE,    "Number of threads to use"},
            {"array_size", required_argument, &array_size, NUMBER, ARRAY_SIZE_FLAG, "Size of the array to sort"},
            {NULL} // Terminating entry
    };


    // Parse the flags
    parse_flags(argc, argv, flags, sizeof(flags) / sizeof(flags[0]));

    uint64_t array[array_size];

    // set up the threads
    omp_set_num_threads(threads);
    GROUP_MIN_DIST = 128;

    // create a pointer function which is used for setup (get array pointer and array size then call POPULATE_ARR on it)
    void (*setup)(uint64_t *, size_t) = &populate_wrapper;
    bool (*verify)(uint64_t *, size_t) = &verify_wrapper;

    // benchmark the quicksort_task_parallelism
    bench_function(quicksort_task_parallelism, array, array_size, 100, HOARE, setup, verify);

}
