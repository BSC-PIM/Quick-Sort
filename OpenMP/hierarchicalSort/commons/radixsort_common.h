#ifndef DPULEVEL_RADIXSORT_COMMON_H
#define DPULEVEL_RADIXSORT_COMMON_H


#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>

#include "config.h"

#define byte uint8_t
#define BYTE_SIZE 8 // each byte is equal to 8 bit


byte create_histograms(uint16_t buckets_n, size_t *cnt, T *array, size_t start, size_t end, uint8_t level,
                       bool is_parallel);

byte get_byte_at(T number, int at);

H get_h_at(T number);

void prefix_sum(size_t start, const size_t *cnt, uint8_t max, size_t *gh, size_t *gt);


#endif //DPULEVEL_RADIXSORT_COMMON_H
