#include "util/queue.h"
#include "util/assert.h"

#include <stdlib.h>

queue_t *queue_create(void)
{
    queue_t *q = NULL;

    q = calloc(1, sizeof(queue_t));
    if (!q) {
        goto done;
    }

    ASSERT_ALWAYS(pthread_mutex_init(&q->mutex, NULL) != 0);
done:
    return q;
}

void queue_destroy(queue_t *q)
{
    if (q) {
        ASSERT_ALWAYS(pthread_mutex_destroy(&q->mutex) != 0);
        free(q);
        q = NULL;
    }
}

int queue_push(queue_t *q, void *item)
{
    int ret = -1;
    queue_item_t *qi;

    if (!q || !item) {
        goto done;
    }  

    qi = calloc(1, sizeof(queue_item_t));
    if (!qi) {
        goto done;
    }

    qi->data = item;

    ASSERT_ALWAYS(pthread_mutex_lock(&q->mutex));

    if (q->head && q->tail) {
        q->tail->next = qi;
        qi->prev = q->tail;
        q->tail = qi;
    } else {
        q->head = qi;
        q->tail = qi;
    }

    q->size++;

    ASSERT_ALWAYS(pthread_mutex_unlock(&q->mutex));

    ret = 0;
done:
    return ret;
}

int queue_pop(queue_t *q, void **item)
{
    int ret = -1;
    queue_item_t *qi;

    if (!q || !item) {
        goto done;    
    }
    
    ASSERT_ALWAYS(pthread_mutex_lock(&q->mutex));

    if (q->head && q->tail) {
        qi = q->head;
        q->head = qi->next;
        if (q->head) {
            q->head->prev = NULL;
        } else {
            q->tail = NULL;
        }
        *item = qi->data;
        qi->data = NULL;
        free(qi);
        qi = NULL;
        q->size--;
        ret = 0;
    }

    ASSERT_ALWAYS(pthread_mutex_unlock(&q->mutex));   
done:
    return ret;
}
