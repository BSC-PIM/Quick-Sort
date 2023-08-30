#include <stdlib.h>

#include "msb_radix_parallel_in_place.h"
#include "common.h"

#define THREADS_SIZE 6
#define ARRAY_SIZE_FLAG 10000
#define TEST_COUNT 5

void populate_wrapper(T *arr, size_t size) {
    POPULATE_ARR(arr, size, 0xFFFF);
}

bool verify_wrapper(T *arr, size_t size) {
    return verify(arr, size);
}

int main(int argc, char *argv[]) {
    size_t array_size = 64;
    int threads = 6;


    // set up the threads
    omp_set_num_threads(threads);
    RSORT_GROUP_MIN_DIST = 128;
    RUNNER_THREADS_NUM = threads;

    bool (*verify)(T *, size_t) = &verify_wrapper;
    void (*setup)(T *, size_t) = &populate_wrapper;


    for (int k = 0; k < 27; k++) {
        array_size = 1 << k;
        size_t temp = MAX(array_size / 8, 1 << 16);
        RSORT_GROUP_MIN_DIST = MIN(temp, array_size);
        T *array = malloc(sizeof(T) * array_size);
        rsort_bench_function(msb_radix_sort_parallel_in_place, array, array_size, 6, TEST_COUNT, setup, verify);
        free(array);
    }


}
