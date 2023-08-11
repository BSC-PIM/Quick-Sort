//
// Created by halfblood on 8/11/23.
//

#include "common.h"

bool verify(uint64_t *arr, size_t size) {
    for (int i = 0; i < size - 1; i++) {
        if (arr[i] > arr[i + 1]) {
            printf("arr[%d] = %lu > arr[%d] = %lu\n", i, arr[i], i + 1, arr[i + 1]);
            return false;
        }
    }
    return true;
}

void bench_function(bench_func func, uint64_t *arr, size_t size, size_t test_count, partition_method_t method,
                    setup_func setup, verify_func verify) {
    double start, end, avg = 0;
    for (int i = 0; i < test_count; i++) {
        setup(arr, size); // Call the setup function before benchmarking
        start = omp_get_wtime();
        func(arr, size, method);
        end = omp_get_wtime();
        avg += (end - start);
        bool verified = verify(arr, size); // Call the verification function
        if (!verified) {
            printf("Verification failed!\n");
        }
    }
    avg /= (double) test_count;

    printf("function took %f in average\n", avg);
}


void quicksort_baseline(uint64_t *arr, size_t size, partition_method_t method) {
    quicksort_rec_seq(arr, 0, size - 1, method);
}