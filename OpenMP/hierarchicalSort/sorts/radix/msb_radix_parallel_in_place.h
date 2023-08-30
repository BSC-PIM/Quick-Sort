#ifndef DPULEVEL_MSB_RADIX_PARALLEL_IN_PLACE_H
#define DPULEVEL_MSB_RADIX_PARALLEL_IN_PLACE_H

#include <stddef.h>
#include <math.h>
#include <malloc.h>
#include <string.h>
#include <omp.h>

#include "config.h"
#include "utils.h"
#include "radixsort_common.h"
#include "msb_radix_sequential_in_place.h"

extern uint16_t RUNNER_THREADS_NUM;

extern size_t RSORT_GROUP_MIN_DIST;

extern size_t RSORT_MAX_BYTE_THRESHOLD;


void msb_radix_sort_parallel_in_place(T *array, size_t size, uint8_t level);

#endif
