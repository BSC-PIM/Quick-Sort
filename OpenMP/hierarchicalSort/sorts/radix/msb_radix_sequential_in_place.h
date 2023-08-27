#ifndef DPULEVEL_MSB_RADIX_SEQUENTIAL_IN_PLACE_H
#define DPULEVEL_MSB_RADIX_SEQUENTIAL_IN_PLACE_H


#include <string.h>

#include "utils.h"
#include "radixsort_common.h"

void mrssip_step(uint64_t *array, size_t start, size_t end, uint8_t level);

void msb_radix_sort_sequential_in_place(uint64_t *array, size_t size, uint8_t level);

#endif
