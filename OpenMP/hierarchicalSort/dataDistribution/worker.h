//
// Created by halfblood on 8/28/23.
//

#ifndef DPULEVEL_WORKER_H
#define DPULEVEL_WORKER_H

#include <stddef.h>
#include "stdint.h"
#include "sort_prop.h"


typedef struct worker {
    size_t me;
    size_t mem_size; // worker_max_memory
    double *w_timer; // time_in_use
    void (*sort_and_tick)(struct worker self, const sort_props_t);
} worker_t;

typedef void (*to_execute)(worker_t, const sort_props_t);

static inline void
init_workers(worker_t *workers, size_t workers_num, size_t worker_max_memory, to_execute worker_psort_and_tick) {
    for (int i = 0; i < workers_num; i++) {
        // set worker id
        workers[i].me = i;

        // set worker max memory
        workers[i].mem_size = worker_max_memory;

        // set worker timer
        workers[i].w_timer = malloc(sizeof(double));
        *(workers[i].w_timer) = 0;

        // set worker to_execute
        workers[i].sort_and_tick = worker_psort_and_tick;
    }
}


/**
 * @brief Perform parallel sorting on a worker's workload and update the timer.
 *
 * This function performs parallel sorting on the given worker's workload using a thread pool.
 * It checks if the worker's memory size is sufficient for the workload, starts a timer,
 * performs the sorting operation, and updates the timer with the sorting duration.
 *
 * @param self The worker structure representing the current worker.
 * @param props Sorting properties for the worker's workload.
 */
static inline void
worker_psort_and_tick(worker_t self, const sort_props_t props) {
    // check if the worker buffer element_in_partition is enough for the workload
    if (self.mem_size < props.element_in_partition) {
        fprintf(stderr, "ERROR: WORKER %zu WORKLOAD LARGER THAN WORKER SIZE", self.me);
        exit(1);
    }

    // start timer
    double start, end;
    start = omp_get_wtime();
    quicksort_threadpool_parallelism(props.input, props.element_in_partition);
    end = omp_get_wtime();
    *(props.timer) = end - start;
    *(self.w_timer) += *(props.timer);
}

#endif
