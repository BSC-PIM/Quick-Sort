#include <stdint.h>
#include <stdlib.h>
#include <omp.h>

#include "config.h"
#include "quicksort_common.h"
#include "benchmarks/commons/common.h"
#include "quicksort_mt.h"
#include "host.h"
#include "radix/two_step_sort.h"


#define TEST_COUNT 50

int main(int argc, char *argv[]) {
    size_t elem = 1 << 24;
    T limit = T_MAX;
    omp_set_num_threads(omp_get_num_procs());

    T *array = (T *) malloc(elem * sizeof(T));
    T *output = (T *) malloc(elem * sizeof(T));

    if (array == NULL || output == NULL) {
        fprintf(stderr, "ALLOCATION FAILED\n");
        exit(1);
    }


    POPULATE_ARR(array, elem, limit);

    int test = TEST_COUNT;

    double host_avg = 0, worker_avg = 0, total_avg = 0;
    for (int i = 0; i < test; i++) {
        host_t host;
        host.worker_count = 640;
        host.worker_mem_size = (1 << 20); // 64MB
        host.thread_count = 6;
        host.host_mem_size = (1 << 30); // 1GB
        host.tasklet_count = 6;

        two_step_sort(&host, array, output, elem);

        // verify if the array is sorted
        for (size_t k = 0; k < elem - 1; k++) {
            if (array[k] > array[k + 1]) {
                printf("NOT SORTED\n");
                break;
            }
        }

        host_avg += host.timer[0];
        worker_avg += host.timer[1];
        total_avg += (host.timer[0] + host.timer[1]);

        printf("-------------------------\n");
        printf("HOST EXECUTION TIME : %f\n", host.timer[0] * 1000);
        printf("WORKER EXECUTION TIME : %f\n", host.timer[1] * 1000);
        printf("TOTAL EXECUTION TIME : %f\n", (host.timer[1] + host.timer[0]) * 1000);
        printf("-------------------------\n");

        POPULATE_ARR(array, elem, limit);
    }

    host_avg = host_avg / test;
    worker_avg = worker_avg / test;
    total_avg = total_avg / test;

    // Set text color to green
    printf("\x1b[32m"); // ANSI escape code for green

    // Print header
    printf("\n");
    printf("|     Metric     |   Average   |\n");
    // Print averages with formatting
    printf("|      Host      | %10.2fs |\n", host_avg);
    printf("|     Worker     | %10.2fs |\n", worker_avg);
    printf("|     Total      | %10.2fs |\n", total_avg);
    printf("\n");
}