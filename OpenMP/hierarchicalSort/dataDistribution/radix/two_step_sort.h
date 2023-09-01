#ifndef DPULEVEL_TWO_STEP_SORT_H
#define DPULEVEL_TWO_STEP_SORT_H


#include <stdint.h>
#include <stdlib.h>
#include <omp.h>

#include "config.h"
#include "quicksort_common.h"
#include "radixsort_common.h"
#include "../sorts/quicksort/quicksort_mt.h"
#include "../sorts/radix/msb_radix_parallel_in_place.h"
#include "../worker.h"
#include "../host.h"


void two_step_sort(host_t *host, T *workload, size_t workload_size);


#endif //DPULEVEL_TWO_STEP_SORT_H
