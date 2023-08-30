#include "partition_and_merge.h"


void init_props(host_t *host, T *workload, size_t workload_size, sort_props_t *s1, double *sort_timer);

void merge(const host_t *host, T *output, size_t workload_size, const sort_props_t *s1, T **ptrs,
           size_t sorted_index);

void worker_psort_and_tick(worker_t self, sort_props_t props) {
    // check if the worker buffer partition_size is enough for the workload
    if (self.mem_size < props.partition_size) {
        fprintf(stderr, "ERROR: WORKER %zu WORKLOAD LARGER THAN WORKER SIZE", self.me);
        exit(1);
    }

    // start timer
    double start, end;
    start = omp_get_wtime();
    quicksort_threadpool_parallelism(props.input, props.partition_size);
    end = omp_get_wtime();
    *(props.timer) = end - start;
    *(self.w_timer) += *(props.timer);
}


void partition_and_merge(host_t *host, T *workload, T *output, size_t workload_size) {
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

    // init workers
    worker_t *workers = malloc(sizeof(worker_t) * host->worker_count);
    init_workers(workers, host->worker_count, host->worker_mem_size, &worker_psort_and_tick);

    // create initial workload
    sort_props_t s1[host->worker_count];
    double sort_timer[host->worker_count];
    init_props(host, workload, workload_size, s1, sort_timer);

    // parallel partition sorting
    double max_worker_time = 0;
    for (int j = 0; j < host->worker_count; j++) {
        workers[j].sort_and_tick(workers[j], s1[j]);
        max_worker_time = MAX(max_worker_time, sort_timer[j]);
    }

    // set pointer on each partition
    T *ptrs[host->worker_count];
    for (int i = 0; i < host->worker_count; i++) {
        ptrs[i] = s1[i].input;
    }

    double start, end;
    start = omp_get_wtime();
    size_t sorted_index = 0;
    // merge all the elements
    merge(host, output, workload_size, s1, ptrs, sorted_index);

    end = omp_get_wtime();
    host->timer[0] = end - start;
    host->timer[1] = max_worker_time;
}

void merge(const host_t *host, T *output, size_t workload_size, const sort_props_t *s1, T **ptrs,
           size_t sorted_index) {
    while (sorted_index != workload_size) {
        T current_min = T_MAX;
        size_t ptr_index = 0;
        for (int m = 0; m < host->worker_count; m++) {
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

            if (ptrs[ptr_index] > (s1[ptr_index].input + s1[ptr_index].partition_size)  )

            if (ptrs[ptr_index] - s1[ptr_index].input == s1[ptr_index].partition_size) {
                ptrs[ptr_index] = NULL;
            }
        }
    }
}


void init_props(host_t *host, T *workload, size_t workload_size, sort_props_t *s1, double *sort_timer) {
    T division_value = workload_size / host->worker_count;
    T reminder = workload_size % host->worker_count;
    for (size_t i = 0; i < host->worker_count; i++) {
        sort_props_t *props = &s1[i];
        props->timer = &sort_timer[i];

        props->type = IN_PLACE;
        props->partition_size = division_value + (i < reminder ? 1 : 0);
        props->input = &workload[i * division_value + (i < reminder ? i : reminder)];
        props->output = NULL;
    }
}