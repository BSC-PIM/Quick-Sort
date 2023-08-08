//
// Created by halfblood on 8/8/23.
//

#include <malloc.h>
#include "work_queue.h"


void work_queue_init(work_queue_t *queue) {
    queue->size = 0;
    queue->head = NULL;
    pthread_mutex_init(&queue->lock, NULL);
    pthread_cond_init(&queue->cond, NULL);
}

void work_queue_push(work_queue_t *queue, job_t *job) {
    // check if queue is not null
    if (queue == NULL) return;

    // create new item
    work_queue_item_t *item = malloc(sizeof(work_queue_item_t));
    item->job = job;

    // lock queue
    pthread_mutex_lock(&queue->lock);

    // add item to queue
    if (queue->head == NULL) {
        queue->head = item;
        item->next = NULL;
        item->prev = NULL;
    } else {
        queue->head->next = item;
        item->prev = queue->head;
        queue->head = item;
    }

    // increase size
    queue->size++;

    // signal to a thread that queue is not empty
    pthread_cond_signal(&queue->cond);

    // unlock queue
    pthread_mutex_unlock(&queue->lock);
}

job_t *work_queue_pop(work_queue_t *queue) {

    // check if queue is not null
    if (queue == NULL) return NULL;

    // lock queue
    pthread_mutex_lock(&queue->lock);

    // wait until queue is not empty (Not Busy Waiting)
    while (queue->size == 0) {
        pthread_cond_wait(&queue->cond, &queue->lock);
    }

    // get last item
    job_t *job = queue->head->job;

    // remove item from queue
    work_queue_item_t *new_head = queue->head->prev;
    if (new_head != NULL)
        new_head->next = NULL;

    queue->head = new_head;

    // decrease the size
    queue->size--;

    pthread_mutex_unlock(&queue->lock);

    return job;
}


void work_queue_destroy(work_queue_t *queue) {
    work_queue_item_t *item = queue->head;
    while (item) {
        work_queue_item_t *temp = item->prev;
        free(item);
        item = temp;
    }
    free(queue);
}