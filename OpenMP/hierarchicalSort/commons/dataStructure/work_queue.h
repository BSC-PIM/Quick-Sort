#ifndef MERGEIDEA_WORK_QUEUE_H
#define MERGEIDEA_WORK_QUEUE_H


#include <stdbool.h>
#include "job.h"
#include "stdint.h"
#include "pthread.h"

extern int WQ_THREAD_NUM;


/// @brief work_queue_item_t is a struct which contains 
typedef struct work_queue_item {
    void *job;
    struct work_queue_item *next;
    struct work_queue_item *prev;
} work_queue_item_t;

/// @brief queue data structure
typedef struct work_queue {
    size_t size;
    work_queue_item_t *head;
    pthread_mutex_t lock;
    pthread_cond_t cond;
    size_t total_pop;
    _Atomic size_t total_processed;
    bool terminate;
} work_queue_t;

/// @brief work_queue_init is a function which initializes work_queue_t
/// @param queue is a pointer to work_queue_t
void work_queue_init(work_queue_t *queue);

/// @brief work_queue_push is a function which pushes job to work_queue_t
/// @note this function is thread safe
/// @param queue is a pointer to work_queue_t
/// @param job is a pointer to job_t
void work_queue_push(work_queue_t *queue, void *job);

/// @brief work_queue_pop is a function which pops job from work_queue_t
/// @note this function is thread safe
/// @param queue is a pointer to work_queue_t
/// @return pointer to job_t
void * work_queue_pop(work_queue_t *queue);

/// @brief work_queue_destroy is a function which destroys work_queue_t
/// @param queue is a pointer to work_queue_t
void work_queue_destroy(work_queue_t *queue);


#endif //MERGEIDEA_WORK_QUEUE_H
