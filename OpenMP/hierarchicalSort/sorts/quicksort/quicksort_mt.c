#include "quicksort_mt.h"


size_t QSORT_GROUP_MIN_DIST = 128;

void task_serve(uint64_t *array, size_t start, size_t end, partition_method_t method);

/**
 * \brief Performs parallel quicksort on an array using task-based parallelism.
 *
 * This function performs parallel quicksort on an array using task-based parallelism.
 * It initializes the parallel region and creates a single task to serve as the entry point for the sorting algorithm.
 * The array is divided into sub-arrays using the Hoare partition scheme, and sorting tasks are created for each subarray.
 * The tasks are then executed by multiple threads, allowing for parallel sorting.
 *
 * \param array Pointer to the array that will be sorted.
 * \param size The size of the array.
 */
void quicksort_task_parallelism(uint64_t *array, size_t size) {
#pragma omp parallel default(none) shared(array, size)
    {
#pragma omp single
        {
            task_serve(array, 0, size - 1, HOARE);
        }
    }
}

/**
 * \brief Performs recursive task-based quicksort on a subarray.
 *
 * This function performs recursive task-based quicksort on a subarray of the original array.
 * If the subarray partition_size is smaller than a certain threshold, sequential quicksort is performed instead.
 * Otherwise, the subarray is divided into two parts using the Hoare partition scheme, and sorting tasks are created
 * for each part. The tasks are then executed by multiple threads in a parallel region.
 *
 * \param array Pointer to the array.
 * \param start The starting index of the subarray.
 * \param end The ending index of the subarray.
 */
void task_serve(uint64_t *array, size_t start, size_t end, partition_method_t method) {
    if (start >= end) return;

    if (end - start < QSORT_GROUP_MIN_DIST) {
        quicksort_rec_seq(array, start, end, method);
        return;
    }

    size_t pivot = partition(array, start, end, method);

#pragma omp task default(none) shared(array, start, pivot, method)
    {
        task_serve(array, start, pivot - 1, method);
    }
#pragma omp task default(none) shared(array, pivot, end, method)
    {
        task_serve(array, pivot, end, method);
    }

#pragma omp taskwait
}


void prepare_queue(size_t size, work_queue_t **wq);

void thread_pool_serve(work_queue_t *wq, uint64_t *array, partition_method_t method);


void quicksort_threadpool_parallelism(uint64_t *array, size_t size) {

    work_queue_t *wq;
    prepare_queue(size, &wq);
    size_t processed = 0;

#pragma omp parallel default(none) shared(array, wq, size, processed)
    {
        thread_pool_serve(wq, array, HOARE);
    }
}


/**
 * \brief Serves tasks from a work queue, processing array segments using a partition method.
 *
 * \param wq Pointer to the work queue from which tasks will be popped and processed.
 * \param array Pointer to the array that will be partitioned or processed by the tasks.
 * \param method The partition method to be used by the `partition` function.
 */
void thread_pool_serve(work_queue_t *wq, uint64_t *array, partition_method_t method) {
    job_t *job;
    while ((job = work_queue_pop(wq)) != NULL) {

        if (job->start >= job->end) {
            wq->total_processed++;
            continue;
        }

        if (job->end - job->start < QSORT_GROUP_MIN_DIST) {
            quicksort_rec_seq(array, job->start, job->end, method);
            wq->total_processed++;
            continue;
        }


        size_t pivot = partition(array, job->start, job->end, method);


        job_t *left, *right;
        left = malloc(sizeof(job_t));
        left->start = job->start;
        left->end = pivot - 1;
        work_queue_push(wq, left);

        right = malloc(sizeof(job_t));
        right->start = pivot;
        right->end = job->end;
        work_queue_push(wq, right);

        free(job);
        wq->total_processed++;
    }
}

/**
 * \brief Initializes the work queue and prepares the first job for processing.
 *
 * The <b>prepare_queue</b> function initializes the work queue and prepares the first job to be processed. It sets up the
 * work queue with the specified thread count and allocates memory for the work queue structure. It also creates the initial
 * job, which represents the entire range of elements to be processed.
 *
 * \param size The size of the array.
 * \param wq A pointer to the pointer of the work queue.
 *
 * \note The work queue and the initial job are modified and allocated within this function.
 */
void prepare_queue(size_t size, work_queue_t **wq) {
    // Initialize the work queue
    *wq = malloc(sizeof(work_queue_t));
    work_queue_init(*wq);

    // Prepare the first job
    job_t *job = malloc(sizeof(job_t));
    job->start = 0;
    job->end = size - 1;

    // Push the first job to the work queue
    work_queue_push(*wq, job);
}