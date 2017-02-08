#ifndef SCANR_WORKPOOL_H
#define SCANR_WORKPOOL_H

#include "util/queue.h"

#include <pthread.h>

typedef void(*workpool_func_t)(void*,void*);
typedef void(*workpool_free_func_t)(void*);

typedef enum {
    WORKPOOL_ACTION_RUN = 0,
    WORKPOOL_ACTION_PAUSE,
    WORKPOOL_ACTION_STOP,
    WORKPOOL_ACTION_DRAIN
} workpool_action_t;

typedef struct {
    pthread_mutex_t mutex;
    pthread_t *workers;
    size_t num_workers;
    workpool_func_t do_func;
    workpool_free_func_t free_func;
    queue_t *queue;
    void *data;
    workpool_action_t action; 
} workpool_t;

/**
 * @brief Create a workpool.
 *
 * @param[in] do_func - Function to do work
 * @param[in] free_func - Function to free item
 * @param[in] num_workers - Number of workers
 * @param[in] q - Pointer to queue_t
 * @param[in] data - User data passed to do_func
 *
 * @return Pointer to workpool_t or NULL on error
 */
workpool_t *workpool_create(workpool_func_t do_func,
                            workpool_free_func_t free_func,
                            size_t num_workers,
                            queue_t *q,
                            void *data);

/**
 * @brief Pause worker threads from starting any new work.
 * 
 * @param[in] wp - Pointer to workpool_t
 */
void workpool_pause(workpool_t *wp);

/**
 * @brief Resume worker threads to start new work.
 *
 * @param[in] wp - Pointer to workpool_t
 */
void workpool_resume(workpool_t *wp);

/**
 * @brief Wait until the queue is empty and signal the worker threads to
 *        finish.
 *
 * @param[in] wp - Pointer to workpool_t
 */
void workpool_drain(workpool_t *wp);

/**
 * @brief Wait until the worker threads finish.
 *
 * @param[in] wp - Pointer to workpool_t
 *
 */
void workpool_stop(workpool_t *wp);

/**
 * @brief Destroy workpool.
 *
 * @param[in] - Pointer to a workspool_t
 */
void workpool_destroy(workpool_t *wp);

#endif
