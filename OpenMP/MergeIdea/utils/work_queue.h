#ifndef MERGEIDEA_WORK_QUEUE_H
#define MERGEIDEA_WORK_QUEUE_H


#include "job.h"
#include "stdint.h"
#include "pthread.h"


// struct for work queue which is thread safe

/// @brief work_queue_item_t is a struct which contains 
typedef struct work_queue_item {
    job_t *job;
    struct work_queue_item *next;
    struct work_queue_item *prev;
} work_queue_item_t;


typedef struct work_queue {
    uint64_t size;
    work_queue_item_t *head;
    pthread_mutex_t lock;
    pthread_cond_t cond;
} work_queue_t;

/// @brief work_queue_init is a function which initializes work_queue_t
/// @param queue is a pointer to work_queue_t
void work_queue_init(work_queue_t *queue);

/// @brief work_queue_push is a function which pushes job to work_queue_t
/// @note this function is thread safe
/// @param queue is a pointer to work_queue_t
/// @param job is a pointer to job_t
void work_queue_push(work_queue_t *queue, job_t *job);

/// @brief work_queue_pop is a function which pops job from work_queue_t
/// @note this function is thread safe
/// @param queue is a pointer to work_queue_t
/// @return pointer to job_t
job_t *work_queue_pop(work_queue_t *queue);

/// @brief work_queue_destroy is a function which destroys work_queue_t
/// @param queue is a pointer to work_queue_t
void work_queue_destroy(work_queue_t *queue);





#endif //MERGEIDEA_WORK_QUEUE_H
