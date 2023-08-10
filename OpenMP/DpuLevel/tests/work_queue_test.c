#include <malloc.h>
#include <omp.h>
#include <stdlib.h>

#include "unity.h"
#include "work_queue.h"


#define JOBS_N 10

work_queue_t *wq;

void setUp() {
    srand(time(0));
    wq = malloc(sizeof(work_queue_t));
    work_queue_init(wq);
}

void tearDown() {
    work_queue_destroy(wq);
}

/// \brief populate_wq is a function which create random jobs
/// \param jobs  is a pointer to array of jobs
/// \param size  is a size of array
static void populate_wq(work_queue_t *wq, job_t **jobs, size_t size) {
    for (size_t i = 0; i < size; i++) {
        jobs[i] = malloc(sizeof(job_t));
        jobs[i]->start = i;
        work_queue_push(wq, jobs[i]);
    }
}

void test_initialWorkQueue_shouldHaveZeroSizeAndNullHEAD(void) {
    TEST_ASSERT_EQUAL_INT64_MESSAGE(wq->size, 0, "SIZE NOT INITIALIZED CORRECTLY");
    TEST_ASSERT_NULL_MESSAGE(wq->head, "INVALID HEAD POINTER");
    TEST_ASSERT_EQUAL_INT64(wq->size, 0);
}

void test_multiplePushOnQueue_shouldInsertAllItemsInOrder(void) {
    job_t *jobs[JOBS_N];
    populate_wq(wq, jobs, JOBS_N);

    for (int i = JOBS_N - 1; i >= 0; i--) {
        TEST_ASSERT_EQUAL_INT64_MESSAGE(work_queue_pop(wq)->start, i, "INCORRECT APPEND");
    }

}

void test_singlePopFromQueue_ShouldReturnTheSameJob(void) {
    job_t *job_push;
    job_push = malloc(sizeof(job_t));

    work_queue_push(wq, job_push);

    job_t *job_pop = work_queue_pop(wq);
    TEST_ASSERT_EQUAL_PTR_MESSAGE(job_push, job_pop, "INCORRECT JOB POP");
}


void test_multiplePopFromQueue_ShouldReturnTheSameJob(void) {
    size_t pushed = 0, popped = 0;
    for (int i = 0; i < JOBS_N * 2; i++) {
        // get a random number between 0 and 1
        if (i % 2 != 0) {
            // pop
            work_queue_pop(wq);
            // atomic increment
            popped++;
        } else {
            // push
            job_t *job = malloc(sizeof(job_t));
            job->start = i;
            work_queue_push(wq, job);
            // atomic increment
            pushed++;
        }
    }

    TEST_ASSERT_EQUAL_INT64_MESSAGE(wq->size, 0, "INCORRECT PUSH AND POP");
}

void test_PopFromEmptyQueue_ShouldReturnNull(void) {
    TEST_ASSERT_NULL_MESSAGE(work_queue_pop(wq), "INCORRECT JOB POP");
}

void test_multiThreadPush_shouldPushAllTheElements(void) {
    int num_threads = omp_get_max_threads();
    job_t *jobs[num_threads];

#pragma omp parallel for default(none) firstprivate(num_threads) shared(jobs, wq)
    for (int i = 0; i < num_threads; i++) {
        jobs[i] = malloc(sizeof(job_t));
        jobs[i]->start = i;
        work_queue_push(wq, jobs[i]);
    }

    // check the size of the queue
    TEST_ASSERT_EQUAL_INT64_MESSAGE(wq->size, num_threads, "RACE CONDITION ON PUSH");
}


void test_multiThreadPop_shouldPopAllTheElements(void) {
    size_t pushed = 0, popped = 0;
#pragma omp parallel for default(none)  shared( wq, pushed, popped) schedule(static, 1)
    for (int i = 0; i < 200; i++) {
        // get a random number between 0 and 1
        if (i % 2 == 0) {
            // pop
            work_queue_pop(wq);
            // atomic increment
#pragma omp atomic
            popped++;
        } else {
            // push
            job_t *job = malloc(sizeof(job_t));
            work_queue_push(wq, job);
            // atomic increment
#pragma omp atomic
            pushed++;
        }
    }

    // check the size of the queue
    TEST_ASSERT_EQUAL_INT64_MESSAGE(200, pushed + popped, "RACE CONDITION ON POP! ");
    TEST_ASSERT_EQUAL_INT64_MESSAGE(wq->size, pushed - popped, "RACE CONDITION ON POP");
    TEST_ASSERT_NULL_MESSAGE(wq->head, "INCORRECT JOB POP");
    TEST_ASSERT_EQUAL_INT64_MESSAGE(wq->total_pop, popped, "INCORRECT JOB POP");
}

int main() {
    UNITY_BEGIN();

    // SEQ TESTS
    RUN_TEST(test_initialWorkQueue_shouldHaveZeroSizeAndNullHEAD);
    RUN_TEST(test_multiplePushOnQueue_shouldInsertAllItemsInOrder);
    RUN_TEST(test_singlePopFromQueue_ShouldReturnTheSameJob);
    RUN_TEST(test_multiplePopFromQueue_ShouldReturnTheSameJob);

    // PARALLEL TESTS
    RUN_TEST(test_multiThreadPush_shouldPushAllTheElements);
    RUN_TEST(test_multiThreadPop_shouldPopAllTheElements);


    UNITY_END();
}

