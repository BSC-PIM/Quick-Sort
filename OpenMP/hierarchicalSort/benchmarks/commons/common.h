#ifndef DPULEVEL_COMMON_H
#define DPULEVEL_COMMON_H

#include <stdint.h>
#include <stddef.h>
#include <stdio.h>
#include <omp.h>
#include "utils.h"
#include <stdlib.h>
#include <stdbool.h>

#include "quicksort_common.h"
#include "radixsort_common.h"


bool verify(const uint64_t *arr, size_t size);


typedef void (*setup_func)(uint64_t *arr, size_t size);

typedef bool (*verify_func)(uint64_t *arr, size_t size);

typedef void (qsort_bench_func)(uint64_t *, size_t size);

typedef void (rsort_bench_func)(uint64_t *, size_t size, uint8_t level);



void
qsort_bench_function(qsort_bench_func qsort, uint64_t *arr, size_t size, size_t test_count, setup_func setup,
                     verify_func verify);

void
rsort_bench_function(rsort_bench_func rsort, uint64_t *arr, size_t size, uint8_t level, size_t test_count,
                     setup_func setup, verify_func verify);



#endif //DPULEVEL_COMMON_H
