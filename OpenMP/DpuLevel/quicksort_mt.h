#ifndef MERGEIDEA_QUICKSORT_MT_TP
#define MERGEIDEA_QUICKSORT_MT_TP


#include <stddef.h>
#include <stdint.h>
#include <omp.h>
#include <malloc.h>


#include "work_queue.h"
#include "quicksort_common.h"

extern size_t GROUP_MIN_DIST;

void quicksort_task_parallelism(uint64_t *array, size_t size, partition_method_t method);

void quicksort_threadpool_parallelism(uint64_t *array, size_t size, partition_method_t method);

#endif
