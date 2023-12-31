#include <stdlib.h>
#include <getopt.h>

#include "quicksort_mt.h"
#include "common.h"
#include "flag_parser.h"

#define TEST_COUNT 50

/**
 * @brief Populate an array with a specified value.
 *
 * This function populates an array with a given value up to the specified size.
 * The array is populated with the value specified by the maximum value of the data type.
 *
 * @param arr Pointer to the array to be populated.
 * @param size Size of the array.
 *
 * @note The array should have enough memory allocated to accommodate the specified size.
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
    size_t array_size = 1000000;
    int threads = 6, group_min_dist = 128;

    // Define the compile flags
    struct Flag flags[] = {
            {.name = "array-size", .has_arg=required_argument, .val='a', .save=&array_size, .type= INT64_T},
            {.name = "threads", .has_arg=required_argument, .val='t', .save=&threads, .type = INT16_T},
            {.name = "group-min-dist", .has_arg=required_argument, .val='g', .save=&group_min_dist, .type=INT16_T}
    };


    // Usage
    const char *Usage = "Usage: program_name [OPTIONS]\n\n"
                        "Description:\n"
                        "  This program demonstrates effects of switching condition between sequential and parallel in sorting.\n\n"
                        "Options:\n"
                        "--threads NUM                         Number of threads to use\n"
                        "--array-size NUM                      Size of the array\n"
                        "--group-min-dist NUM                  Minimum size required for a partition to run in parallel\n"
                        "Usage Examples:\n"
                        "  program_name --threads 4 --array-size 100  --group-min-dist 128 # Run with 4 threads and array size 100 and partition-min-dist 128\n";

    parse_flags(argc, argv, flags, sizeof(flags) / sizeof(flags[0]), Usage);
    T *array = malloc(sizeof(T) * array_size);

    // set up the threads
    omp_set_num_threads(threads);
    QSORT_GROUP_MIN_DIST = group_min_dist;


    qsort_bench_function(quicksort_task_parallelism, array, array_size, TEST_COUNT, &populate_wrapper, &verify_wrapper);

}
