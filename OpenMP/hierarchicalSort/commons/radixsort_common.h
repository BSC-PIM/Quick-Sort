#ifndef DPULEVEL_RADIXSORT_COMMON_H
#define DPULEVEL_RADIXSORT_COMMON_H


#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>

#define byte uint8_t
#define BYTE_SIZE 8 // each byte is equal to 8 bit


byte create_histograms(uint16_t buckets_n, size_t *cnt, uint64_t *array, size_t start, size_t end, uint8_t level,
                       bool is_parallel);

byte get_byte_at(uint64_t number, int at);


#endif //DPULEVEL_RADIXSORT_COMMON_H
