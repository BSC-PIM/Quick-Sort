#include "radixsort_common.h"

/**
 * @brief Extracts a byte from a typeof(T) number at the specified position.
 *
 * This function extracts a byte from the given typeof(T) number at the specified
 * position. The position is counted from the most significant byte (MSB) side,
 * starting with 0 for the MSB itself.
 *
 * @param number The typeof(T) number from which to extract the byte.
 * @param at The position of the byte to extract (0 for MSB, 7 for LSB).
 * @return The extracted byte.
 *
 * @note The @p at parameter is oriented from the most significant byte (MSB) side,
 *       where 0 represents the MSB itself and higher values represent bytes moving
 *       towards the least significant byte (LSB).
 */
typedef uint8_t byte;

inline byte get_byte_at(T number, int at) {
    uint8_t available_bytes = sizeof(number);
//    if (at > available_bytes - 1) {
//        fprintf(stderr, "Invalid byte position\n");
//        exit(EXIT_FAILURE);
//    }

    byte result = (number >> ((available_bytes - at - 1) * 8)) & 0xFF;
    return result;
}


inline H get_h_at(T number) {
//    if (sizeof(T) < sizeof(H)) {
//        fprintf(stderr, "Invalid byte position\n");
//        exit(EXIT_FAILURE);
//    }
    return (number >> ((sizeof(T) - sizeof(H)) * 8));
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
inline byte create_histograms(uint16_t buckets_n, size_t *cnt, T *array, size_t start, size_t end, uint8_t level,
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


/**
 * @brief Compute prefix sum for bucket indices based on counts.
 *
 * This function computes the prefix sum for bucket indices using the given count array `cnt`.
 * The computed prefix sums are stored in the `gh` (start indices) and `gt` (end indices) arrays.
 *
 * @param start The start index for the prefix sum computation.
 * @param cnt Pointer to the array containing bucket counts.
 * @param max Maximum byte value encountered in the array.
 * @param gh Pointer to the array for storing start indices.
 * @param gt Pointer to the array for storing end indices.
 */
void prefix_sum(size_t start, const size_t *cnt, uint8_t max, size_t *gh, size_t *gt) {
    gh[0] = start;
    gt[0] = start + cnt[0];

    // Use prefix sum for cnt
    for (size_t i = 1; i < max + 1; i++) {
        gh[i] = gt[i - 1];
        gt[i] = gt[i - 1] + cnt[i];
    }
}