#include <stdint.h>
#include <stdlib.h>
#include <omp.h>
#include "quicksort_common.h"
#include "commons/common.h"
#include "../quicksort_mt.h"


#define TEST_COUNT 50
#define WORKER_NUM 64


void sort_and_tick(uint64_t *arr, size_t size, partition_method_t method, double *time) {


    double start, end;
    start = omp_get_wtime();
    quicksort_task_parallelism(arr, size, method);
    end = omp_get_wtime();
    *time = (end - start);
}


typedef struct worker {
    size_t buffer_size;
    uint64_t *buffer;
    double time;

    void (*sort_and_tick)(uint64_t *, size_t, partition_method_t, double *);
} worker_t;


uint64_t get_num_from_worker(size_t *ptr) {
    uint64_t num = *ptr;
    return num;
}


int main(int argc, char *argv[]) {
    int threads = 6;
    size_t buffer_size = 1000000;

    // set up the threads
    omp_set_num_threads(threads);

    // init buffer
    uint64_t buffer[buffer_size];

    // init workers
    size_t temp = buffer_size;
    worker_t workers[WORKER_NUM];
    for (int i = 0; i < WORKER_NUM; i++) {
        if (temp < buffer_size / WORKER_NUM) {
            workers[i].buffer_size = temp;
        } else {
            workers[i].buffer_size = buffer_size / WORKER_NUM;
            temp -= buffer_size / WORKER_NUM;
        }

        workers[i].buffer = &buffer[i * workers[i].buffer_size];
        workers[i].sort_and_tick = &sort_and_tick;
    }


    double start, end, avg_total = 0, avg_worker = 0, avg_master = 0;
    for (int k = 0; k < TEST_COUNT; k++) {
        // alloc a buffer
        size_t *output = (size_t *) malloc(sizeof(size_t) * buffer_size);

        // populate buffer
        POPULATE_ARR(buffer, buffer_size, UINT64_MAX);

        // parallel sort_and_tick
        double max_time = 0, time;
        for (int j = 0; j < WORKER_NUM; j++) {
            workers[j].sort_and_tick(workers[j].buffer, workers[j].buffer_size, HOARE, &time);
            max_time = MAX(max_time, time);
        }

        avg_worker += max_time;


        size_t *ptrs[WORKER_NUM];
        for (int i = 0; i < WORKER_NUM; i++) {
            ptrs[i] = &buffer[i * workers[i].buffer_size];
        }

        start = omp_get_wtime();
        size_t sorted_index = 0;
        while (sorted_index != buffer_size) {
            uint64_t current_min = UINT64_MAX;
            size_t ptr_index = 0;
            for (int m = 0; m < WORKER_NUM; m++) {
                if (ptrs[m] != NULL) {
                    uint64_t num = get_num_from_worker(ptrs[m]);
                    if (num < current_min) {
                        current_min = num;
                        ptr_index = m;
                    }
                }
            }
            output[sorted_index] = current_min;
            sorted_index++;

            if (ptrs[ptr_index] != NULL) {
                ptrs[ptr_index]++;
                if (ptrs[ptr_index] - workers[ptr_index].buffer == workers[ptr_index].buffer_size) {
                    ptrs[ptr_index] = NULL;
                }
            }
        }
        end = omp_get_wtime();
        avg_master += (end - start);
        avg_total += (end - start) + max_time;
        verify(output, buffer_size);
        free(output);
    }
    avg_total /= TEST_COUNT;
    avg_worker /= TEST_COUNT;
    avg_master /= TEST_COUNT;
    printf("%f, ", avg_worker * 1000);
    printf("%f\n", avg_master * 1000);


}