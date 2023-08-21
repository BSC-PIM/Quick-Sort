#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>

#include "quicksort_mt.h"
#include "common.h"
#include "flag_parser.h"

#define ARRAY_SIZE_FLAG 1000000
#define TEST_COUNT 50
#define GROUP_MIN_DIST_FLAG 1

void populate_wrapper(uint64_t *arr, size_t size) {
    POPULATE_ARR(arr, size, INTMAX_MAX);
}

bool verify_wrapper(uint64_t *arr, size_t size) {
    return verify(arr, size);
}

int main(int argc, char *argv[]) {
    int array_size, group_min_dist;
    array_size = ARRAY_SIZE_FLAG;
    group_min_dist = GROUP_MIN_DIST_FLAG;
    uint64_t array[array_size];

    // set up the threads
    omp_set_num_threads(omp_get_num_procs());
    GROUP_MIN_DIST = group_min_dist;

    // create a pointer function which is used for setup (get array pointer and array size then call POPULATE_ARR on it)
    void (*setup)(uint64_t *, size_t) = &populate_wrapper;
    bool (*verify)(uint64_t *, size_t) = &verify_wrapper;

    printf("\n");
    for (int i = 0; i < 12; i++) {
        GROUP_MIN_DIST = 1 << i;
        bench_function(quicksort_task_parallelism, array, array_size, TEST_COUNT, HOARE, setup, verify);
    }

}
