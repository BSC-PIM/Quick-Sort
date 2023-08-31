#include "two_step_sort.h"

uint16_t get_corresponding_worker_id(size_t index, size_t mem_size);


void two_step_sort(host_t *host, T *workload, T *output, size_t workload_size) {

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

    // first step sort : sort elements based on the H MSB bit
    // set number of threads
    omp_set_num_threads(host->thread_count);
    RUNNER_THREADS_NUM = host->thread_count;
    double start, end;
    start = omp_get_wtime();
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

    s1[0].type = IN_PLACE;
    s1[0].input = workload;
    s1[0].output = NULL;
    s1[0].timer = &sort_timer[0];
    s1[0].element_in_partition = 0;

    uint16_t cwp_id = 0;
    uint16_t mj_count = 0;

    size_t worker_mem_size_elems = host->worker_mem_size / sizeof(T);
    for (size_t bucket_idx = 0; bucket_idx < bucket_len; bucket_idx++) {
        size_t elem_in_bucket = buckets[bucket_idx];
        buckets[bucket_idx] += bucket_idx == 0 ? 0 : buckets[bucket_idx - 1];
        if (elem_in_bucket == 0) continue;

        if (s1[cwp_id].element_in_partition == (worker_mem_size_elems)) {
            cwp_id++;
            s1[cwp_id].input = s1[cwp_id - 1].input + s1[cwp_id - 1].element_in_partition;
            s1[cwp_id].timer = &sort_timer[cwp_id];
            s1[cwp_id].element_in_partition = 0;
            s1[cwp_id].type = IN_PLACE;
            s1[cwp_id].output = NULL;
        }

        uint16_t wp_id = get_corresponding_worker_id(buckets[bucket_idx], host->worker_mem_size);

        if (wp_id - 1 > cwp_id) {
            merge_job_t job;
            job.w_first = cwp_id;
            job.w_last = wp_id;
            job.start = bucket_idx == 0 ? 0 : buckets[bucket_idx - 1];

            size_t cwp_space = worker_mem_size_elems - s1[cwp_id].element_in_partition;

            size_t remaining_current_bucket_len = elem_in_bucket - cwp_space;
            s1[cwp_id].element_in_partition = worker_mem_size_elems;

            for (uint16_t worker_idx = cwp_id + 1; worker_idx < wp_id; worker_idx++) {
                s1[worker_idx].type = IN_PLACE;
                s1[worker_idx].output = NULL;

                s1[worker_idx].input = s1[worker_idx - 1].input + s1[worker_idx - 1].element_in_partition;
                s1[worker_idx].timer = &sort_timer[worker_idx];

                s1[worker_idx].element_in_partition = MIN(remaining_current_bucket_len, worker_mem_size_elems);
                remaining_current_bucket_len -= s1[worker_idx].element_in_partition;
            }

            job.end = buckets[bucket_idx];
            m1[mj_count] = job;
            mj_count++;
            cwp_id = wp_id - 1;
        } else {
            s1[cwp_id].element_in_partition += elem_in_bucket;
        }
    }

    end = omp_get_wtime();
    host->timer[0] += end - start;


    // parallel partition sorting
    double max_worker_time = 0;
    for (uint16_t i = 0; i <= cwp_id; i++) {
        workers[i].sort_and_tick(workers[i], s1[i]);
        max_worker_time = MAX(max_worker_time, sort_timer[i]);
    }
    printf("max worker time is equal to : %f\n", max_worker_time * 1000);


    // do the merge step based on the number of workers
    for (uint16_t i = 0; i < mj_count; i++) {
        start = omp_get_wtime();
        merge_job_t job = m1[i];
        size_t merge_size = job.end;
        size_t sorted_index = job.start;

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
            output[sorted_index] = current_min;
            sorted_index++;

            if (ptrs[ptr_index] != NULL) {
                ptrs[ptr_index]++;

                if (ptrs[ptr_index] ==
                    (s1[job.w_first + ptr_index].input + s1[job.w_first + ptr_index].element_in_partition))
                    ptrs[ptr_index] = NULL;

            }
        }
        end = omp_get_wtime();
        host->timer[0] += end - start;

        // copy the output to the workload
        memcpy(workload + job.start, output + job.start, (job.end - job.start) * sizeof(T));
    }

    host->timer[1] = max_worker_time;
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




