#include <stdio.h>
#include <omp.h>
#include <malloc.h>
#include "utils/work_queue.h"


int main() {

    work_queue_t *wq = malloc(sizeof(work_queue_t));
    work_queue_init(wq);

    int num_threads = omp_get_max_threads();

#pragma omp parallel for default(none) firstprivate(num_threads)
    for (int i = 0; i < num_threads; i++) {
        printf("THREAD FROM THREAD POOL WITH ID : %d \n", omp_get_thread_num());
    }

}
