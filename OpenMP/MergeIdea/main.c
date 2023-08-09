#include <stdio.h>
#include <stdlib.h>
#include "utils.h"

#include "quicksort_mt_tp.h"


#define CACHE_LINE_SIZE 128
#define ELEM_N 10
#define THREAD_NUM 2
#define PARALLEL 1


int main() {
    int max_threads = omp_get_max_threads();
    QS_THREAD_NUM = MIN(max_threads, THREAD_NUM);

    uint64_t *array = malloc(sizeof(uint64_t) * ELEM_N);
    POPULATE_ARR(array, ELEM_N);
    PRINT_ARR(array, ELEM_N);
    quicksort_multi_thread_thread_pool(array, ELEM_N);
    PRINT_ARR(array, ELEM_N);
}
