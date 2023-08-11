//
// Created by halfblood on 8/11/23.
//

#ifndef DPULEVEL_COMMON_H
#define DPULEVEL_COMMON_H

#include <stdint.h>
#include <stddef.h>
#include <stdio.h>
#include <omp.h>
#include <utils.h>
#include <stdlib.h>
#include <stdbool.h>

#include "quicksort_common.h"


/* VERIFICATION */
bool verify(uint64_t *arr, size_t size);

typedef void (bench_func)(uint64_t *, size_t size, partition_method_t method);


typedef void (*setup_func)(uint64_t *arr, size_t size);

typedef bool (*verify_func)(uint64_t *arr, size_t size);


void bench_function(bench_func func, uint64_t *arr, size_t size, size_t test_count, partition_method_t method,
                    setup_func setup, verify_func verify);

void quicksort_baseline(uint64_t *arr, size_t size, partition_method_t method);


#endif //DPULEVEL_COMMON_H
