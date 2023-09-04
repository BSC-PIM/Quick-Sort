#include "two_step_sort.h"

uint16_t get_corresponding_worker_id(size_t index, size_t mem_size);


void
partition_data(const host_t *host, uint32_t *workload, size_t bucket_len, size_t *buckets, double *sort_timer,
               sort_props_t *s1, merge_job_t *m1, uint16_t *cwp_id, uint16_t *mj_count);

void merge_partitions(host_t *host, uint32_t *workload, double start, double end, const sort_props_t *s1,
                      const merge_job_t *m1, uint16_t mj_count);

/**
 * @brief Perform a two-step sorting process.
 *
 * This function performs a two-step sorting process on the given workload using multiple threads.
 * The workload is sorted first based on the most significant bits (H MSB) and then partitioned and merged.
 * Timers are used to measure the execution time of each step.
 *
 * @param host Pointer to the host structure containing host information.
 * @param workload Pointer to the input workload array to be sorted.
 * @param workload_size Size of the workload array.
 */
void two_step_sort(host_t *host, T *workload, size_t workload_size) {

    // check if host has enough memory
    if (host->host_mem_size < workload_size * sizeof(T)) {
        fprintf(stderr, "Host does not have enough memory to run partition_and_merge\n");
        exit(1);
    }

    // check if number of available threads is enough
    if (omp_get_num_procs() < host->thread_count) {
        fprintf(stderr, "Host does not have enough threads to run partition_and_merge\n");
        exit(1);
    }

    // restart timers
    host->timer[0] = 0;
    host->timer[1] = 0;


    // init workers
    worker_t *workers = malloc(sizeof(worker_t) * host->worker_count);
    init_workers(workers, host->worker_count, host->worker_mem_size, &worker_psort_and_tick);

    double start = 0, end = 0;
    start = omp_get_wtime();

    // first level sort
    msb_radix_sort_parallel_in_place(workload, workload_size, 0); // this will only sort the H most-significant bits

    size_t bucket_len = 1 << (sizeof(H) * 8);
    size_t *buckets = calloc(sizeof(size_t), bucket_len);
    // iterate over the semi-sorted arrays and set the partitions
    for (size_t i = 0; i < workload_size; i++) {
        H h = get_h_at(workload[i]);
        buckets[h]++;
    }

    // create workloads
    double sort_timer[host->worker_count];
    sort_props_t s1[host->worker_count];
    merge_job_t m1[host->worker_count];

    uint16_t cwp_id;
    uint16_t mj_count;

    // partition data
    partition_data(host, workload, bucket_len, buckets, sort_timer, s1, m1, &cwp_id, &mj_count);

    end = omp_get_wtime();
    host->timer[0] += end - start;

    // parallel partition sorting
    double max_worker_time = 0;
    for (uint16_t i = 0; i <= cwp_id; i++) {
        workers[i].sort_and_tick(workers[i], s1[i]);
        max_worker_time = MAX(max_worker_time, sort_timer[i]);
    }

    // do the merge step based on the number of workers
    merge_partitions(host, workload, start, end, s1, m1, mj_count);

    host->timer[1] = max_worker_time;
}

/**
 * @brief Merge sorted partitions of data.
 *
 * This function merges sorted partitions of data into a single sorted array.
 * It performs the merging efficiently and updates the host's timer.
 *
 * @param host Pointer to the host structure containing host information.
 * @param workload Pointer to the input workload array.
 * @param start Start time for measuring the merge operation.
 * @param end End time for measuring the merge operation.
 * @param s1 Array of sorting properties for workers.
 * @param m1 Array of merge job descriptions.
 * @param mj_count Number of merge jobs to execute.
 */
void merge_partitions(host_t *host, uint32_t *workload, double start, double end, const sort_props_t *s1,
                      const merge_job_t *m1, uint16_t mj_count) {
    for (uint16_t i = 0; i < mj_count; i++) {
        start = omp_get_wtime();
        merge_job_t job = m1[i];
        size_t merge_size = job.end;
        size_t sorted_index = job.start;

        T *temp = malloc(sizeof(T) * (job.end - job.start));

        uint16_t ptr_size = job.w_last - job.w_first;
        T *ptrs[ptr_size - 1];
        ptrs[0] = workload + job.start;
        for (uint16_t j = job.w_first + 1; j < job.w_last; j++) {
            ptrs[j - job.w_first] = s1[j].input;
        }

        while (sorted_index != merge_size) {
            T current_min = T_MAX;
            size_t ptr_index = 0;
            for (uint16_t j = 0; j < ptr_size; j++) {
                if (ptrs[j] != NULL) {
                    T num = *(ptrs[j]);
                    if (num < current_min) {
                        current_min = num;
                        ptr_index = j;
                    }
                }
            }
            temp[sorted_index - job.start] = current_min;
            sorted_index++;

            if (ptrs[ptr_index] != NULL) {
                ptrs[ptr_index]++;

                if (ptrs[ptr_index] ==
                    (s1[job.w_first + ptr_index].input + s1[job.w_first + ptr_index].element_in_partition))
                    ptrs[ptr_index] = NULL;

            }
        }
        end = omp_get_wtime();

        // copy the output to the workload
        memcpy(workload + job.start, temp, (job.end - job.start) * sizeof(T));
        free(temp);
        host->timer[0] += end - start;
    }
}

/**
 * @brief Partition and organize data for parallel sorting and merging.
 *
 * This function partitions and organizes the input data into suitable chunks for parallel sorting and merging.
 * It determines the distribution of elements to workers, creates sorting properties, and generates merge jobs
 * to efficiently utilize the available resources.
 *
 * @param host Pointer to the host structure containing host information.
 * @param workload Pointer to the input workload array.
 * @param bucket_len Length of the buckets array.
 * @param buckets Array of bucket sizes.
 * @param sort_timer Array for storing sorting timers.
 * @param s1 Array of sorting properties for workers.
 * @param m1 Array of merge job descriptions.
 * @param cwp_id Pointer to the current worker pointer ID.
 * @param mj_count Pointer to the merge job count.
 */
void partition_data(const host_t *host, uint32_t *workload, size_t bucket_len, size_t *buckets, double *sort_timer,
                    sort_props_t *s1, merge_job_t *m1, uint16_t *cwp_id, uint16_t *mj_count) {
    (*cwp_id) = 0;
    (*mj_count) = 0;
    s1[0].type = IN_PLACE;
    s1[0].input = workload;
    s1[0].output = NULL;
    s1[0].timer = &sort_timer[0];
    s1[0].element_in_partition = 0;
    size_t worker_mem_size_elems = host->worker_mem_size / sizeof(T);
    for (size_t bucket_idx = 0; bucket_idx < bucket_len; bucket_idx++) {
        size_t elem_in_bucket = buckets[bucket_idx];
        buckets[bucket_idx] += bucket_idx == 0 ? 0 : buckets[bucket_idx - 1];
        if (elem_in_bucket == 0) continue;

        if (s1[(*cwp_id)].element_in_partition == (worker_mem_size_elems)) {
            (*cwp_id)++;
            s1[(*cwp_id)].input = s1[(*cwp_id) - 1].input + s1[(*cwp_id) - 1].element_in_partition;
            s1[(*cwp_id)].timer = &sort_timer[(*cwp_id)];
            s1[(*cwp_id)].element_in_partition = 0;
            s1[(*cwp_id)].type = IN_PLACE;
            s1[(*cwp_id)].output = NULL;
        }

        uint16_t wp_id = get_corresponding_worker_id(buckets[bucket_idx], host->worker_mem_size);

        if (wp_id - 1 > (*cwp_id)) {
            merge_job_t job;
            job.w_first = (*cwp_id);
            job.w_last = wp_id;
            job.start = bucket_idx == 0 ? 0 : buckets[bucket_idx - 1];

            size_t cwp_space = worker_mem_size_elems - s1[(*cwp_id)].element_in_partition;

            size_t remaining_current_bucket_len = elem_in_bucket - cwp_space;
            s1[(*cwp_id)].element_in_partition = worker_mem_size_elems;

            for (uint16_t worker_idx = (*cwp_id) + 1; worker_idx < wp_id; worker_idx++) {
                s1[worker_idx].type = IN_PLACE;
                s1[worker_idx].output = NULL;

                s1[worker_idx].input = s1[worker_idx - 1].input + s1[worker_idx - 1].element_in_partition;
                s1[worker_idx].timer = &sort_timer[worker_idx];

                s1[worker_idx].element_in_partition = MIN(remaining_current_bucket_len, worker_mem_size_elems);
                remaining_current_bucket_len -= s1[worker_idx].element_in_partition;
            }

            job.end = buckets[bucket_idx];
            m1[(*mj_count)] = job;
            (*mj_count)++;
            (*cwp_id) = wp_id - 1;
        } else {
            s1[(*cwp_id)].element_in_partition += elem_in_bucket;
        }
    }
}


/**
 * @brief Get the worker id using the element index in workload
 *
 * This function gets the index of the element in buffer and hte memory size of the workers and computer which
 * worker must have this element.
 *
 * @param index Index of the element in the workload
 * @param mem_size Size of the workers memory in byte
 * @return index of the worker containing the element with `index`
 */
uint16_t get_corresponding_worker_id(size_t index, size_t mem_size) {
    // Calculate the number of elements that can be stored in each worker's memory
    size_t elements_per_worker = mem_size / sizeof(T);

    // Calculate the worker id based on the index and elements per worker
    uint16_t worker_id = CEILING(index, elements_per_worker);

    return worker_id;
}