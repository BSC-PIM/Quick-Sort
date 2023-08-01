//
// Created by halfblood on 7/28/23.
//

#ifndef BASELINE_QUICKSORT_H
#define BASELINE_QUICKSORT_H

#include <stdint-gcc.h>
#include <stdlib.h>

typedef enum {
    RANDOM,
    FIRST,
    LAST,
    MEDIAN,
    PREDETERMINED
} pivot_type;


void quicksort(uint32_t *arr, int low, int high, pivot_type type);

#endif //BASELINE_QUICKSORT_H
