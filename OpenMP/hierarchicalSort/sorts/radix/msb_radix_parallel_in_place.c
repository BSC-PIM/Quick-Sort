#include "msb_radix_parallel_in_place.h"


uint16_t RUNNER_THREADS_NUM = 6;

extern size_t RSORT_GROUP_MIN_DIST = 128;

extern size_t RSORT_MAX_BYTE_THRESHOLD = 64;


/**
 * @brief Partition buckets among threads using a balanced strategy.
 *
 * This function partitions the buckets of an array based on specified indices `gh` and `gt` for each bucket.
 * The buckets are partitioned among multiple threads using a balanced partitioning strategy.
 *
 * @param gh Pointer to the array of starting indices for each bucket.
 * @param gt Pointer to the array of ending indices for each bucket.
 * @param max Maximum byte value encountered in the array.
 * @param threads_num Number of threads for partitioning.
 * @param processors_buckets_head 2D array storing the head indices of buckets for each thread.
 * @param processors_buckets_tail 2D array storing the tail indices of buckets for each thread.
 *
 * @note The arrays `gh`, `gt`, `processors_buckets_head`, and `processors_buckets_tail` should have enough memory allocated.
 */
static void partition_buckets(const size_t *gh, const size_t *gt, byte max, size_t threads_num,
                              size_t processors_buckets_head[threads_num][max + 1],
                              size_t processors_buckets_tail[threads_num][max + 1]) {

    // Iterate over threads for partitioning
    for (size_t tid = 0; tid < threads_num; tid++) {
#pragma omp task default(none) firstprivate(tid, max, threads_num, gh, gt) shared(processors_buckets_head, processors_buckets_tail)
        {
            // Check if the thread id is the last one
            bool is_last_thread = tid == threads_num - 1;

            // Partition the buckets among the threads using tid
            // Balanced partitioning of each bucket
            for (size_t i = 0; i < max + 1; i++) {
                processors_buckets_head[tid][i] = gh[i] + ((gt[i] - gh[i]) * tid) / threads_num;

                if (is_last_thread) {
                    processors_buckets_tail[tid][i] = gt[i];
                } else {
                    processors_buckets_tail[tid][i] = gh[i] + ((gt[i] - gh[i]) * (tid + 1)) / threads_num;
                }
            }
        }
    }

#pragma omp taskwait
}


/**
 * @brief Permute elements within sub-buckets of an array.
 *
 * This function permutes elements within sub-buckets of an array based on specified bucket indices `gh` and `gt`.
 * It reorders elements using a radix-based approach to achieve sorting.
 *
 * @param array Pointer to the array to be permuted.
 * @param max Maximum byte value encountered in the array.
 * @param level Byte level used for permute operation.
 * @param threads_num Number of threads for parallel processing.
 * @param processors_buckets_head 2D array storing the head indices of buckets for each thread.
 * @param processors_buckets_tail 2D array storing the tail indices of buckets for each thread.
 *
 * @note `processors_buckets_head`, and `processors_buckets_tail` should have enough memory allocated.
 */
static void permute(T *array, byte max, uint8_t level, uint16_t threads_num,
                    size_t processors_buckets_head[threads_num][max + 1],
                    size_t processors_buckets_tail[threads_num][max + 1]) {


    for (uint16_t tid = 0; tid < threads_num; tid++) {
#pragma omp task  default(none) firstprivate(tid, max, level, threads_num) shared(processors_buckets_head, processors_buckets_tail, array)
        {
            T temp;
            byte temp_b;
            for (size_t i = 0; i < max + 1; i++) {
                size_t head = processors_buckets_head[tid][i];
                while (head < processors_buckets_tail[tid][i]) {
                    temp = array[head];
                    temp_b = get_byte_at(temp, level);
                    while (temp_b != (byte) i &&
                           processors_buckets_head[tid][temp_b] < processors_buckets_tail[tid][temp_b]) {
                        SWAP(temp, array[processors_buckets_head[tid][temp_b]]);
                        processors_buckets_head[tid][temp_b]++;
                        temp_b = get_byte_at(temp, level);
                    }
                    if (temp_b == (byte) i) {
                        array[head] = array[processors_buckets_head[tid][temp_b]];
                        head++;
                        array[processors_buckets_head[tid][temp_b]] = temp;
                        processors_buckets_head[tid][temp_b]++;
                    } else {
                        array[head] = temp;
                        head++;
                    }
                }
            }
        }
    }

#pragma omp taskwait
}


/**
 * @brief Repair and reorder elements in an array based on specified bucket indices.
 *
 * This function repairs and reorders elements within an array using a radix-based approach. It ensures that
 * elements are correctly positioned according to the provided bucket indices `gh` and `gt`.
 *
 * @param array Pointer to the array to be repaired and reordered.
 * @param gh Pointer to the array of starting indices for each bucket.
 * @param gt Pointer to the array of ending indices for each bucket.
 * @param max Maximum byte value encountered in the array.
 * @param level Byte level used for repair and reordering.
 * @param threads_num Number of threads for parallel processing.
 * @param processors_buckets_head 2D array storing the head indices of buckets for each thread.
 * @param processors_buckets_tail 2D array storing the tail indices of buckets for each thread.
 * @return The count of buckets that are completely repaired.
 *
 * @note The arrays `gh`, `gt`, `processors_buckets_head`, and `processors_buckets_tail` should have enough memory allocated.
 */
static void repair(T *array, size_t *gh, const size_t *gt, byte max, uint8_t level, uint16_t threads_num,
                   size_t processors_buckets_head[threads_num][max + 1],
                   size_t processors_buckets_tail[threads_num][max + 1]) {

    for (uint16_t i = 0; i < max + 1; i++) {
#pragma omp task if(max > RSORT_MAX_BYTE_THRESHOLD) default(none) firstprivate(threads_num) shared(gt, processors_buckets_head, processors_buckets_tail, gh, array, level, i)
        {
            size_t tail = gt[i];
            // iterate over all the threads
            for (int tid = 0; tid < threads_num; tid++) {
                size_t head = processors_buckets_head[tid][i];
                while (head < processors_buckets_tail[tid][i] && head < tail) {
                    T temp = array[head];
                    byte temp_b = get_byte_at(temp, level);
                    if (temp_b != (byte) i) {
                        while (head < tail) {
                            tail--;
                            T w = array[tail];
                            byte w_b = get_byte_at(w, level);
                            if (w_b == (byte) i) {
                                array[head] = w;
                                array[tail] = temp;
                                break;
                            }
                        }
                    }
                    head++;
                }
            }
            gh[i] = tail;
        }
    }

#pragma omp taskwait
}


/**
 * @brief Sort a portion of an array in-place using a radix-based approach with parallelism.
 *
 * This function sorts a specified portion of an array using a radix-based approach. It employs parallelism to
 * accelerate the sorting process.
 *
 * @param array Pointer to the array to be sorted.
 * @param start Index indicating the start of the portion to be sorted.
 * @param end Index indicating the end of the portion to be sorted (exclusive).
 * @param level Byte level used for sorting.
 * @param threads_num Number of threads for parallel processing.
 */
void mrpip_step(T *array, size_t start, size_t end, uint8_t level, uint16_t threads_num) {


    // if size is less than RSORT_GROUP_MIN_DIST, use sequential sorting
    if (end - start < RSORT_GROUP_MIN_DIST) {
        mrssip_step(array, start, end, level);
        return;
    }


    uint16_t buckets_n = 1 << BYTE_SIZE;
    size_t cnt[buckets_n];
    memset(cnt, 0, buckets_n * sizeof(size_t));

    byte max = create_histograms(buckets_n, cnt, array, start, end, level, true);

    size_t gh[max + 1], gt[max + 1];
    prefix_sum(start, cnt, max, gh, gt);

    while (1) {
        size_t processors_buckets_head[threads_num][max + 1];
        size_t processors_buckets_tail[threads_num][max + 1];

        partition_buckets(gh, gt, max, threads_num, processors_buckets_head, processors_buckets_tail);
        permute(array, max, level, threads_num, processors_buckets_head, processors_buckets_tail);
        repair(array, gh, gt, max, level, threads_num, processors_buckets_head, processors_buckets_tail);

        bool is_finished = true;
        for (uint16_t i = 0; i < max + 1; i++)
            if (gh[i] != gt[i]) {
                is_finished = false;
                break;
            }

        if (is_finished) break;
    }

    if (level < sizeof(array) - 1) {
        for (int i = 0; i < max + 1; i++) {
            size_t s = gt[i] - cnt[i];
            size_t e = gt[i];


#pragma omp task shared(array) default(none) firstprivate(i, level, threads_num, s, e)
            {
                mrpip_step(array, s, e, level + 1, threads_num);
            }
        }
    }
}


void msb_radix_sort_parallel_in_place(T *array, size_t size, uint8_t level) {
#pragma omp parallel default(none) shared(array, size, level, RUNNER_THREADS_NUM)
    {
#pragma omp single
        {
            mrpip_step(array, 0, size, level, RUNNER_THREADS_NUM);
        }
    }
}




