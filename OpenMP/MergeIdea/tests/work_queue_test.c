#include <malloc.h>
#include "unity.h"
#include "../utils/work_queue.h"

#define JOBS_N 10

work_queue_t *wq;

void setUp() {
    wq = malloc(sizeof(work_queue_t));
    work_queue_init(wq);
}

void tearDown() {
    work_queue_destroy(wq);
}

void test_initialWorkQueue_shouldHaveZeroSizeAndNullHEAD(void) {
    TEST_ASSERT_EQUAL_INT64_MESSAGE(wq->size, 0, "SIZE NOT INITIALIZED CORRECTLY");
    TEST_ASSERT_NULL_MESSAGE(wq->head, "INVALID HEAD POINTER");
    TEST_ASSERT_EQUAL_INT64(wq->size, 0);
}

void test_multiplePushOnQueue_shouldInsertAllItemsInOrder(void) {
    job_t jobs[JOBS_N];
    for (int i = 0; i < JOBS_N; i++) {
        jobs[i].start = i;
        work_queue_push(wq, &jobs[i]);
    }

    for (int i = JOBS_N - 1; i >= 0; i--) {
        TEST_ASSERT_EQUAL_INT64_MESSAGE(work_queue_pop(wq)->start, i, "INCORRECT APPEND");
    }

}


int main() {
    UNITY_BEGIN();
    RUN_TEST(test_initialWorkQueue_shouldHaveZeroSizeAndNullHEAD);
    RUN_TEST(test_multiplePushOnQueue_shouldInsertAllItemsInOrder);
    UNITY_END();
}