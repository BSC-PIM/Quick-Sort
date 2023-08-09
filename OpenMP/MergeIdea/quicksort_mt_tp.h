#ifndef MERGEIDEA_QUICKSORT_MT_TP
#define MERGEIDEA_QUICKSORT_MT_TP


#include <stddef.h>
#include <stdint.h>
#include <omp.h>
#include <malloc.h>


#include "work_queue.h"
#include "quicksort_common.h"


extern int QS_THREAD_NUM;

void quicksort_multi_thread_thread_pool(uint64_t *array, size_t size);

#endif
