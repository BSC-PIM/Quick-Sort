#ifndef MERGEIDEA_QUICKSORT_MT_TP
#define MERGEIDEA_QUICKSORT_MT_TP


#include <stddef.h>
#include <stdint.h>
#include <omp.h>
#include <malloc.h>


#include "config.h"
#include "work_queue.h"
#include "quicksort_common.h"


extern size_t QSORT_GROUP_MIN_DIST;

void quicksort_task_parallelism(T *array, size_t size);

void quicksort_threadpool_parallelism(T *array, size_t size);

#endif
