#include "common.h"

/**
 * @brief Verify if an array is sorted in ascending order.
 *
 * This function checks if the elements of the array are in ascending order up to the specified size.
 * If any element is found to be greater than the subsequent element, an error message is printed, and
 * the function returns `false`, indicating that the array is not sorted.
 *
 * @param arr Pointer to the array to be verified.
 * @param size Size of the array.
 * @return `true` if the array is sorted in ascending order, `false` otherwise.
 *
 * @note The array should have at least `size` elements.
 */
bool verify(const T *arr, size_t size) {
    for (int i = 0; i < size - 1; i++) { ;
        if (arr[i] > arr[i + 1]) {
            return false;
        }
    }
    return true;
}


/**
 * @brief Benchmark a function's performance.
 *
 * This function benchmarks the performance of a given function that operates on an array.
 * The function is benchmarked for a specified number of times (`test_count`).
 * A setup function is called before each benchmark to prepare the array.
 * After calling the benchmarked function, a verification function is called to verify the results.
 * The average execution time across all tests is calculated and printed in milliseconds.
 *
 * @param qsort The function to be benchmarked.
 * @param arr Pointer to the array used for benchmarking.
 * @param size Size of the array.
 * @param test_count Number of times to benchmark the function.
 * @param setup The setup function to prepare the array before each benchmark.
 * @param verify The verification function to validate the results after each benchmark.
 *
 * @note The array should have at least `size` elements.
 * @warning Make sure the setup and verification functions work correctly with the benchmarked function.
 */

void
qsort_bench_function(qsort_bench_func qsort, T *arr, size_t size, size_t test_count, setup_func setup,
                     verify_func verify) {
    double start, end, avg = 0;
    for (int i = 0; i < test_count; i++) {
        setup(arr, size); // Call the setup function before benchmarking
        start = omp_get_wtime();
        qsort(arr, size);
        end = omp_get_wtime();
        avg += (end - start);
        bool verified = verify(arr, size); // Call the verification function
        if (!verified) {
            printf("Verification failed!\n");
            exit(1);
        }
    }
    avg /= (double) test_count;

    printf("%f\n", avg * 1000);
}


void rsort_bench_function(rsort_bench_func rsort, T *arr, size_t size, uint8_t level, size_t test_count,
                     setup_func setup, verify_func verify) {
    double start, end, avg = 0;
    for (int i = 0; i < test_count; i++) {
        setup(arr, size); // Call the setup function before benchmarking
        start = omp_get_wtime();
        rsort(arr, size, level);
        end = omp_get_wtime();
        avg += (end - start);
        bool verified = verify(arr, size); // Call the verification function
        if (!verified) {
            printf("Verification failed!\n");
            exit(1);
        }
    }
    avg /= (double) test_count;

    printf("%f\n", avg * 1000);
}
