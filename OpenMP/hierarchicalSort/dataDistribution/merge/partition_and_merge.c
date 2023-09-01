#include "partition_and_merge.h"


void init_props(host_t *host, T *workload, size_t workload_size, sort_props_t *s1, double *sort_timer);

void merge(const host_t *host, T *output, size_t workload_size, const sort_props_t *s1, T **ptrs);

void check_memory_bound(const host_t *host, size_t workload_size);

/**
 * @brief Perform parallel partition sorting and sequential merging of workloads.
 *
 * This function performs parallel partition sorting and sequential merging of workloads using workers.
 * It checks if the host has enough memory and available threads, initializes workers, creates initial workloads,
 * performs parallel partition sorting, sets pointers for merging, and sequentially merges the sorted sub-arrays into the output array.
 *
 * @param host Pointer to the host structure containing host information.
 * @param workload Pointer to the input workload array.
 * @param output Pointer to the output array for storing the merged sorted elements.
 * @param workload_size Size of the workload and output arrays.
 */
void partition_and_merge(host_t *host, T *workload, T *output, size_t workload_size) {
    // check if host has enough memory
    check_memory_bound(host, workload_size);

    // restart timers
    host->timer[0] = 0;
    host->timer[1] = 0;

    // init workers
    worker_t *workers = malloc(sizeof(worker_t) * host->worker_count);
    init_workers(workers, host->worker_count, host->worker_mem_size, &worker_psort_and_tick);

    // create initial workload
    double start, end;
    start = omp_get_wtime();
    sort_props_t s1[host->worker_count];
    double sort_timer[host->worker_count];
    init_props(host, workload, workload_size, s1, sort_timer);
    end = omp_get_wtime();
    host->timer[0] += end - start;

    // parallel partition sorting
    double max_worker_time = 0;
    for (uint16_t j = 0; j < host->worker_count; j++) {
        workers[j].sort_and_tick(workers[j], s1[j]);
        max_worker_time = MAX(max_worker_time, sort_timer[j]);
    }

    // set pointer on each partition
    T *ptrs[host->worker_count];
    for (uint16_t i = 0; i < host->worker_count; i++) {
        ptrs[i] = s1[i].input;
    }

    start = omp_get_wtime();
    merge(host, output, workload_size, s1, ptrs);
    end = omp_get_wtime();

    host->timer[0] += end - start;
    host->timer[1] += max_worker_time;
}

/**
 * @brief Check if the host has enough memory and available threads.
 *
 * This function checks if the host has enough memory to accommodate the workload and if there are
 * enough available threads to execute parallel operations.
 *
 * @param host Pointer to the host structure containing host information.
 * @param workload_size Size of the workload to be processed.
 */
void check_memory_bound(const host_t *host, size_t workload_size) {
    if (host->host_mem_size < workload_size * sizeof(T)) {
        fprintf(stderr, "Host does not have enough memory to run partition_and_merge\n");
        exit(1);
    }

    // check if number of available threads is enough
    if (omp_get_num_procs() < host->thread_count) {
        fprintf(stderr, "Host does not have enough threads to run partition_and_merge\n");
        exit(1);
    }
}


/**
 * @brief Merge sorted sub-arrays into a single sorted output array.
 *
 * This function merges the sorted sub-arrays from individual workers into a single sorted output array.
 * It iterates through the sub-arrays and selects the smallest element at each step to populate the output array.
 *
 * @param host Pointer to the host structure containing worker count information.
 * @param output Pointer to the output array for storing the merged sorted elements.
 * @param workload_size Size of the workload and output arrays.
 * @param s1 Pointer to an array of sort_props_t structures containing sorting properties for each worker.
 * @param ptrs Pointer to an array of pointers to the current positions in the sub-arrays.
 */
void merge(const host_t *host, T *output, size_t workload_size, const sort_props_t *s1, T **ptrs) {
    size_t sorted_index = 0;
    while (sorted_index != workload_size) {
        T current_min = T_MAX;
        size_t ptr_index = 0;
        for (uint16_t m = 0; m < host->worker_count; m++) {
            if (ptrs[m] != NULL) {
                T num = *(ptrs[m]);
                if (num < current_min) {
                    current_min = num;
                    ptr_index = m;
                }
            }
        }
        output[sorted_index] = current_min;
        sorted_index++;

        if (ptrs[ptr_index] != NULL) {
            ptrs[ptr_index]++;

            if (ptrs[ptr_index] == (s1[ptr_index].input + s1[ptr_index].element_in_partition))
                ptrs[ptr_index] = NULL;
        }
    }
}


/**
 * @brief Initialize sorting properties for parallel sorting.
 *
 * This function initializes sorting properties for parallel sorting using the provided workload
 * and host information. It sets up sorting properties for each worker thread, specifying sorting type,
 * partition size, input data, and output data.
 *
 * @param host Pointer to the host structure containing worker count information.
 * @param workload Pointer to the input workload array.
 * @param workload_size Size of the workload array.
 * @param s1 Pointer to an array of sort_props_t structures for sorting properties.
 * @param sort_timer Pointer to an array of timers for each worker thread.
 *
 * @note This function assumes that the total memory available for all worker threads combined is sufficient to accommodate the workloads.
 */
void init_props(host_t *host, T *workload, size_t workload_size, sort_props_t *s1, double *sort_timer) {
    T division_value = workload_size / host->worker_count;
    T reminder = workload_size % host->worker_count;

    for (size_t i = 0; i < host->worker_count; i++) {
        sort_props_t *props = &s1[i];
        props->timer = &sort_timer[i];

        props->type = IN_PLACE;
        props->element_in_partition = division_value + (i < reminder ? 1 : 0);
        props->input = &workload[i * division_value + (i < reminder ? i : reminder)];
        props->output = NULL;
    }
}

