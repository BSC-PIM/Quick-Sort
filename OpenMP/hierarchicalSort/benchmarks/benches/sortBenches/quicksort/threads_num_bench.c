#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>

#include "quicksort_mt.h"
#include "common.h"
#include "flag_parser.h"

#define TEST_COUNT 100


/**
 * @brief Baseline quicksort function for comparison.
 *
 * This function implements a baseline version of the quicksort algorithm.
 * It operates on an array of typeof(T) elements and sorts them in ascending order.
 *
 * @param arr Pointer to the array to be sorted.
 * @param size Size of the array.
 */
void quicksort_baseline(T *arr, size_t size) {
    quicksort_rec_seq(arr, 0, size - 1, HOARE);
}

/**
 * @brief Populate an array with a specified value.
 *
 * This function populates an array with a given value up to the specified element_in_partition.
 * The array is populated with the value specified by the maximum value of the data type.
 *
 * @param arr Pointer to the array to be populated.
 * @param size Size of the array.
 *
 * @note The array should have enough memory allocated to accommodate the specified element_in_partition.
 * @warning Using this function with large sizes might lead to performance issues.
 */
void populate_wrapper(T *arr, size_t size) {
    POPULATE_ARR(arr, size, T_MAX);
}


/**
 * @brief Verify wrapper function.
 *
 * This function wraps the `verify` function to provide a consistent interface for verification.
 *
 * @param arr Pointer to the array to be verified.
 * @param size Size of the array.
 * @return `true` if the array is verified successfully, `false` otherwise.
 */
bool verify_wrapper(T *arr, size_t size) {
    return verify(arr, size);
}

int main(int argc, char *argv[]) {
    int threads = 6;
    size_t array_size = 1000000;

    // Define the flags
    struct Flag flags[] = {
            {.name = "threads", .has_arg=required_argument, .val='t', .save=&threads, .type = INT16_T},
            {.name = "array-partition-size", .has_arg=required_argument, .val='a', .save=&array_size, .type= INT64_T},
    };


    const char *Usage = "Usage: program_name [OPTIONS]\n\n"
                        "Description:\n"
                        "  This program demonstrates different threads number and method effects on sorting.\n\n"
                        "Options:\n"
                        "--threads NUM                         Number of threads to use\n"
                        "--array-element_in_partition NUM                      Size of the array\n"
                        "Usage Examples:\n"
                        "  program_name --threads 4 --array-element_in_partition 100   # Run with 4 threads and array element_in_partition 100\n";


    // Parse the flags
    parse_flags(argc, argv, flags, sizeof(flags) / sizeof(flags[0]), Usage);

    // TODO implement optional arg
    if (!flags[0].present) {
        printf("use default setup for `thread` flag. threads := %d\n", threads);
    }
    if (!flags[1].present) {
        printf("use default setup for `array-size` flag. array-size := %zu\n", array_size);
    }


    T *array = malloc(sizeof(T) * array_size);

    // set up the threads
    omp_set_num_threads(threads);
    QSORT_GROUP_MIN_DIST = 128;

    // create a pointer function which is used for setup (get array pointer and array size then call POPULATE_ARR on it)
    void (*setup)(T *, size_t) = &populate_wrapper;
    bool (*verify)(T *, size_t) = &verify_wrapper;

    // benchmark the quicksort baseline model (sequential)
    qsort_bench_function(quicksort_baseline, array, array_size, TEST_COUNT, setup, verify);

    // benchmark the quicksort task parallelism model
    qsort_bench_function(quicksort_task_parallelism, array, array_size, TEST_COUNT, setup, verify);

    // benchmark the quicksort threadpool model
    qsort_bench_function(quicksort_threadpool_parallelism, array, array_size, TEST_COUNT, setup, verify);
}