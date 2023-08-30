#include <stdint.h>
#include <stdlib.h>
#include <omp.h>
#include "quicksort_common.h"
#include "benchmarks/commons/common.h"
#include "quicksort_mt.h"
#include "host.h"
#include "merge/partition_and_merge.h"


#define TEST_COUNT 50
#define WORKER_NUM 8

int main(int argc, char *argv[]) {
    size_t elem = 10000000;
    uint64_t limit = UINT64_MAX;
    omp_set_num_threads(6);

    uint64_t *array = (uint64_t *) malloc(elem * sizeof(uint64_t));
    uint64_t *output = (uint64_t *) malloc(elem * sizeof(uint64_t));
    POPULATE_ARR(array, elem, limit);

    int test = 10;

    for (int i = 0; i < test; i++) {
        host_t host;
        host.worker_count = 1;
        host.worker_mem_size = 2000000 * sizeof(uint64_t);
        host.thread_count = 6;
        host.host_mem_size = 10000000 * sizeof(uint64_t);
        host.tasklet_count = 6;

        partition_and_merge(&host, array, output, elem);

        // verify if the array is sorted
        for (size_t k = 0; k < elem - 1; k++) {
            if (output[k] > output[k + 1]) {
                printf("fuck me\n");
                break;
            }
        }

        printf("-------------------------\n");
        printf("HOST EXECUTION TIME : %f\n", host.timer[0] * 1000);
        printf("WORKER EXECUTION TIME : %f\n", host.timer[1] * 1000);
        printf("-------------------------\n");


        POPULATE_ARR(array, elem, limit);

    }
}