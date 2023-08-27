#include "radixsort_common.h"

/**
 * @brief Extracts a byte from a uint64_t number at the specified position.
 *
 * This function extracts a byte from the given uint64_t number at the specified
 * position. The position is counted from the most significant byte (MSB) side,
 * starting with 0 for the MSB itself.
 *
 * @param number The uint64_t number from which to extract the byte.
 * @param at The position of the byte to extract (0 for MSB, 7 for LSB).
 * @return The extracted byte.
 *
 * @note The @p at parameter is oriented from the most significant byte (MSB) side,
 *       where 0 represents the MSB itself and higher values represent bytes moving
 *       towards the least significant byte (LSB).
 */
typedef uint8_t byte;

byte get_byte_at(uint64_t number, int at) {
    int available_bytes = sizeof(number);
    if (at > available_bytes - 1) {
        fprintf(stderr, "Invalid byte position\n");
        exit(EXIT_FAILURE);
    }

    byte result = (number >> ((available_bytes - at - 1) * 8)) & 0xFF;
    return result;
}



/**
 * @brief Create histograms from a portion of an array.
 *
 * This function generates histograms for a given portion of an array. It processes elements
 * within the specified range [start, end) and constructs histograms based on a specified
 * byte level of each element.
 *
 * @param buckets_n Number of histogram buckets.
 * @param cnt Pointer to an array to store histogram counts.
 * @param array Pointer to the input array.
 * @param start Index indicating the start of the portion to process.
 * @param end Index indicating the end of the portion to process (exclusive).
 * @param level Byte level to extract from each element.
 * @param is_parallel Flag indicating whether to use parallel processing.
 * @return The maximum byte value encountered in the array.
 *
 * @note The `cnt` array should have at least `buckets_n` elements allocated.
 */
byte create_histograms(uint16_t buckets_n, size_t *cnt, uint64_t *array, size_t start, size_t end, uint8_t level,
                       bool is_parallel) {
    byte max = 0; // fix max data type

    // OpenMP parallel for loop to process array elements and construct histograms
#pragma omp parallel for if(is_parallel) default(none) firstprivate(buckets_n, start, end, level) reduction(max:max) reduction(+:cnt[:buckets_n]) schedule(static) shared(array)
    for (size_t i = start; i < end; i++) {
        byte ba = get_byte_at(array[i], level);
        if (ba > max) max = ba;
        cnt[ba]++;
    }

    return max;
}