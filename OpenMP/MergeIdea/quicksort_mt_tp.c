#include "quicksort_mt_tp.h"

int QS_THREAD_NUM;


void prepareQueue(size_t size, work_queue_t **wq);

void serve(work_queue_t *wq, uint64_t *array, partition_method_t method);

/**
 * \brief Prepares a work queue for the quicksort algorithm and performs multi-threaded sorting.
 *
 * This function prepares a work queue for the quicksort algorithm and performs sorting using multiple threads.
 * It initializes the work queue and the thread pool, and then distributes the sorting tasks among the threads.
 * The sorting is performed using the Hoare partition scheme.
 *
 * \param array Pointer to the array that will be sorted.
 * \param size The size of the array.
 */
void quicksort_multi_thread_thread_pool(uint64_t *array, size_t size) {

    // Initialize the work queue
    work_queue_t *wq;
    prepareQueue(size, &wq);


#pragma omp parallel default(none) shared(wq, array)
    {
        serve(wq, array, HOARE);
    }
}


/**
 * \brief Serves tasks from a work queue, processing array segments using a partition method.
 *
 * \param wq Pointer to the work queue from which tasks will be popped and processed.
 * \param array Pointer to the array that will be partitioned or processed by the tasks.
 * \param method The partition method to be used by the `partition` function.
 */
void serve(work_queue_t *wq, uint64_t *array, partition_method_t method) {
    job_t *job;
    while ((job = work_queue_pop(wq)) != NULL) {
        printf("THREAD ID %d: %zu - %zu\n", omp_get_thread_num(), job->start, job->end);
        printf("THE ARRAY IS: ");
        PRINT_ARR(array, 10);
        size_t pivot;

        if (job->start >= job->end) continue;

        pivot = partition(array, job->start, job->end, method);

        job_t *left, *right;
        left = malloc(sizeof(job_t));
        left->start = job->start;
        left->end = pivot - 1;
        work_queue_push(wq, left);

        right = malloc(sizeof(job_t));
        right->start = pivot;
        right->end = job->end;
        work_queue_push(wq, right);


    }
}

/**
 * \brief Initializes the work queue and prepares the first job for processing.
 *
 * The <b>prepareQueue</b> function initializes the work queue and prepares the first job to be processed. It sets up the
 * work queue with the specified thread count and allocates memory for the work queue structure. It also creates the initial
 * job, which represents the entire range of elements to be processed.
 *
 * \param size The size of the array.
 * \param wq A pointer to the pointer of the work queue.
 *
 * \note The work queue and the initial job are modified and allocated within this function.
 */
void prepareQueue(size_t size, work_queue_t **wq) {
    // Initialize the work queue
    omp_set_num_threads(QS_THREAD_NUM);
    WQ_THREAD_NUM = QS_THREAD_NUM;
    *wq = malloc(sizeof(work_queue_t));
    work_queue_init(*wq);

    // Prepare the first job
    job_t *job = malloc(sizeof(job_t));
    job->start = 0;
    job->end = size - 1;

    // Push the first job to the work queue
    work_queue_push(*wq, job);
}