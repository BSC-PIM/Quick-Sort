//
// Created by halfblood on 8/9/23.
//

#ifndef MERGEIDEA_QUICKSORT_COMMON_H
#define MERGEIDEA_QUICKSORT_COMMON_H

#include <stddef.h>
#include <stdint.h>

#include "utils.h"
#include "config.h"



typedef enum {
    LOMUTO,
    HOARE
} partition_method_t;

size_t lomuto_partition(T *array, size_t start, size_t end);

size_t hoare_partition(T *array, size_t start, size_t end);

size_t partition(T *array, size_t start, size_t end, partition_method_t method);

void quicksort_rec_seq(T *array, size_t start, size_t end, partition_method_t method);

void quicksort_stack_seq(T *array, size_t start, size_t end, partition_method_t method);

#endif //MERGEIDEA_QUICKSORT_COMMON_H
