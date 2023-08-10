//
// Created by halfblood on 8/9/23.
//

#ifndef MERGEIDEA_QUICKSORT_COMMON_H
#define MERGEIDEA_QUICKSORT_COMMON_H

#include <stddef.h>
#include <stdint.h>
#include "utils.h"


typedef enum {
    LOMUTO,
    HOARE
} partition_method_t;

size_t lomuto_partition(uint64_t *array, size_t start, size_t end);

size_t hoare_partition(uint64_t *array, size_t start, size_t end);

size_t partition(uint64_t *array, size_t start, size_t end, partition_method_t method);

void quicksort_rec_seq(uint64_t *array, uint64_t start, uint64_t end, partition_method_t method);

void quicksort_stack_seq(uint64_t *array, uint64_t start, uint64_t end, partition_method_t method);

#endif //MERGEIDEA_QUICKSORT_COMMON_H
