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
    void (*sort_and_tick)(struct worker self, sort_props_t);
} worker_t;

typedef void (*to_execute)(worker_t, sort_props_t);

static inline void init_workers(worker_t *workers, size_t workers_num, size_t worker_max_memory, to_execute worker_psort_and_tick) {
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

#endif
