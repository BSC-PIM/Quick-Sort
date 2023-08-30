#ifndef DPULEVEL_PARTITION_AND_MERGE_H
#define DPULEVEL_PARTITION_AND_MERGE_H


#include <stdint.h>
#include <stdlib.h>
#include <omp.h>

#include "quicksort_common.h"
#include "../sorts/quicksort/quicksort_mt.h"
#include "../worker.h"
#include "../host.h"

#define T uint64_t

void partition_and_merge(host_t *host, T *workload, T *output, size_t workload_size);

#endif
