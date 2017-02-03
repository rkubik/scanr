#ifndef SCANR_QUEUE_H
#define SCANR_QUEUE_H

#include <pthread.h>
#include <stddef.h>

struct queue_item {
    void *data;
    struct queue_item *prev;
    struct queue_item *next;
};

typedef struct queue_item queue_item_t;

typedef struct {
    pthread_mutex_t mutex;
    size_t size;
    queue_item_t *head;
    queue_item_t *tail;
} queue_t;

/**
 * @brief Create a new queue.
 *
 * @return Pointer to a new queue, or NULL on error
 */
queue_t *queue_create(void);

/**
 * @brief Destroy a queue.
 *
 * @param[in] q - Pointer to a queue_t
 */
void queue_destroy(queue_t *q);

/**
 * @brief Push an item on the queue.
 *
 * @param[in] q - Pointer to a queue_t
 * @param[in] item - Item to add
 *
 * @return 0 on success, -1 on error
 */
int queue_push(queue_t *q, void *item);

/**
 * @brief Pop an item off the queue.
 *
 * @param[in] q - Pointer to a queue_t
 * @param[out] item - Item to remove
 *
 * @return 0 on success, -1 on error
 */
int queue_pop(queue_t *q, void **item);

#endif
