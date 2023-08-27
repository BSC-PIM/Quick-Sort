#include "msb_radix_sequential_in_place.h"

void sort_and_swap(uint64_t *array, uint8_t level, uint8_t max, size_t *gh, const size_t *gt);

void prefix_sum(size_t start, const size_t *cnt, uint8_t max, size_t *gh, size_t *gt);

/**
 * @brief Perform a step of the MRSSIP (Most-Significant Bit Radix Sequential In-Place) sorting algorithm on a portion of an array.
 *
 * This function performs a step of the MRSSIP sorting algorithm on a specified portion of an array. It operates on a given
 * byte level to sort the elements in-place.
 *
 * @param array Pointer to the array to be sorted.
 * @param start Index indicating the start of the portion to be sorted.
 * @param end Index indicating the end of the portion to be sorted (exclusive).
 * @param level Byte level used for sorting.
 */
void mrssip_step(uint64_t *array, size_t start, size_t end, uint8_t level) { // NOLINT(*-no-recursion)

    size_t buckets_n = 1 << BYTE_SIZE;
    size_t cnt[buckets_n];
    memset(cnt, 0, buckets_n * sizeof(size_t));

    byte max = create_histograms(buckets_n, cnt, array, start, end, level, false);

    size_t gh[max + 1], gt[max + 1];
    prefix_sum(start, cnt, max, gh, gt);
    sort_and_swap(array, level, max, gh, gt);

    // Recursively perform the next step for the remaining byte levels
    if (level < sizeof(array) - 1) {
        for (uint16_t i = 0; i < max + 1; i++) {
            mrssip_step(array, gt[i] - cnt[i], gt[i], level + 1);
        }
    }
}

void prefix_sum(size_t start, const size_t *cnt, uint8_t max, size_t *gh, size_t *gt) {
    gh[0] = start;
    gt[0] = start + cnt[0];

    // Use prefix sum for cnt
    for (size_t i = 1; i < max + 1; i++) {
        gh[i] = gt[i - 1];
        gt[i] = gt[i - 1] + cnt[i];
    }
}

void sort_and_swap(uint64_t *array, uint8_t level, uint8_t max, size_t *gh,
                   const size_t *gt) {// Iterate over buckets and sort in-place
    uint64_t temp;
    byte temp_b;
    for (size_t i = 0; i < max + 1; i++) {
        while (gh[i] < gt[i]) {
            temp = array[gh[i]];
            temp_b = get_byte_at(temp, level);
            while (temp_b != (byte) i) {
                SWAP(temp, array[gh[temp_b]]);
                gh[temp_b]++;
                temp_b = get_byte_at(temp, level);
            }
            array[gh[i]++] = temp;
        }
    }
}

/**
 * @brief Perform a Most-Significant Bit Radix Sort (MSB) sequentially in-place on an array.
 *
 * This function performs a Most-Significant Bit Radix Sort on an array sequentially and in-place.
 *
 * @param array Pointer to the array to be sorted.
 * @param size Size of the array.
 */
void msb_radix_sort_sequential_in_place(uint64_t *array, size_t size, uint8_t level) {
    mrssip_step(array, 0, size, level);
}

