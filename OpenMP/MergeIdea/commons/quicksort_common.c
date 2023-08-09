//
// Created by halfblood on 8/9/23.
//

#include "quicksort_common.h"
#include "stdio.h"


/**
 * \brief Performs the Hoare partition algorithm to partition an array around a pivot value.
 *
 * The Hoare partition algorithm rearranges the elements of the array in such a way that all elements lesser than the pivot
 * value are placed before it, and all elements greater than the pivot value are placed after it. The initial pivot value is
 * chosen as the first element of the array.
 *
 * \param array The array to be partitioned.
 * \param start The starting index of the partition range.
 * \param end The ending index of the partition range.
 *
 * \return The final index of the pivot element after partitioning.
 *
 * \note The partitioned elements are modified in-place within the array.
 */
size_t hoare_partition(uint64_t *array, size_t start, size_t end) {


    size_t k, j;
    uint64_t pivot = array[end];
    for (k = start, j = end; 1; k++, j--) {
        while (array[k] < pivot)
            k++;

        while (array[j] > pivot)
            j--;

        if (k >= j)
            return (k);
        SWAP(array[k], array[j]);
    }
}

/**
 * \brief Performs the Lomuto partition algorithm to partition an array around a pivot value.
 *
 * The Lomuto partition algorithm rearranges the elements of the array in such a way that all elements lesser than the pivot
 * value are placed before it, and all elements greater than or equal to the pivot value are placed after it. The initial pivot
 * value is chosen as the last element of the array.
 *
 * \param array The array to be partitioned.
 * \param start The starting index of the partition range.
 * \param end The ending index of the partition range.
 *
 * \return The final index of the pivot element after partitioning.
 *
 * \note The partitioned elements are modified in-place within the array.
 */
size_t lomuto_partition(uint64_t *array, size_t start, size_t end) {
    uint64_t pivot_value = array[end];
    size_t i = start;

    for (size_t j = start; j < end; j++) {
        if (array[j] < pivot_value) {
            SWAP(array[i], array[j]);
            i++;
        }
    }

    SWAP(array[i], array[end]);
    return i;
}

/**
 * \brief Performs the partitioning of an array based on the specified partition method.
 *
 * The `partition` function partitions the elements of the array within the specified range (`start` to `end`) based on the
 * chosen partition method. The partition method can be either Lomuto or Hoare. If an unrecognized partition method is
 * provided, the Lomuto partition method is used by default.
 *
 * \param array The array to be partitioned.
 * \param start The starting index of the partition range.
 * \param end The ending index of the partition range.
 * \param method The partition method to use (either LOMUTO or HOARE).
 *
 * \return The final index of the pivot element after partitioning.
 *
 * \note The partitioned elements are modified in-place within the array.
 */
size_t partition(uint64_t *array, size_t start, size_t end, partition_method_t method) {
    switch (method) {
        case LOMUTO:
            return lomuto_partition(array, start, end);
        case HOARE:
            return hoare_partition(array, start, end);
        default:
            return lomuto_partition(array, start, end);
    }
}

