#include <stdlib.h>

#include "quicksort_mt.h"
#include "common.h"

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
    int threads = 6;


    // set up the threads
    omp_set_num_threads(threads);
    QSORT_GROUP_MIN_DIST = 128;

    bool (*verify)(uint64_t *, size_t) = &verify_wrapper;
    void (*setup)(uint64_t *, size_t) = &populate_wrapper;

    size_t array_size = 2;
    for (int i = 0; i < 16; i++) {
        array_size = array_size << 1;
        uint64_t *array = malloc(sizeof(uint64_t) * array_size);
        qsort_bench_function(quicksort_task_parallelism, array, array_size, TEST_COUNT, setup, verify);
        free(array);
    }


}
