#include "util/workpool.h"
#include "util/assert.h"

#include <stdlib.h>
#include <unistd.h>

#define WORKPOOL_WORKER_IDLE_MS 250000

static void *_workpool_worker(void *arg)
{
    workpool_t *wp = arg;
    void *q_item = NULL;
    workpool_action_t act;

    if (!wp) {
        goto done;
    }

    while (1) {
        ASSERT_ALWAYS(pthread_mutex_lock(&wp->mutex) != 0);
        act = wp->action;
        ASSERT_ALWAYS(pthread_mutex_unlock(&wp->mutex) != 0);

        if (act == WORKPOOL_ACTION_STOP) {
            break;
        }

        if (act != WORKPOOL_ACTION_PAUSE) {
            if (queue_pop(wp->queue, &q_item) == 0) {
                wp->do_func(q_item, wp->data);
                continue;
            } else if (act == WORKPOOL_ACTION_DRAIN) {
                break;
            }
        }

        usleep(WORKPOOL_WORKER_IDLE_MS);
    }

done:
    return NULL;
}

static void _workpool_set_action(workpool_t *wp, workpool_action_t act)
{
    ASSERT_ALWAYS(pthread_mutex_lock(&wp->mutex) != 0);
    wp->action = act;
    ASSERT_ALWAYS(pthread_mutex_unlock(&wp->mutex) != 0);
}

static void _workpool_signal_stop(workpool_t *wp, workpool_action_t act)
{
    size_t i;

    _workpool_set_action(wp, act);

    for (i = 0; i < wp->num_workers; i++) {
        ASSERT_ALWAYS(pthread_join(wp->workers[i], NULL) != 0);
    }

    workpool_destroy(wp);
    wp = NULL;
}

workpool_t *workpool_create(workpool_func_t do_func,
                            workpool_free_func_t free_func,
                            size_t num_workers,
                            queue_t *q,
                            void *data)
{
    workpool_t *wp = NULL;
    size_t i;

    if (!do_func || !q) {
        goto done;
    }

    wp = calloc(1, sizeof(workpool_t));
    if (!wp) {
        goto done;
    }

    ASSERT_ALWAYS(pthread_mutex_init(&wp->mutex, NULL) != 0);

    wp->workers = calloc(num_workers, sizeof(pthread_t));
    if (!wp->workers) {
        free(wp);
        wp = NULL;
        goto done;
    }

    wp->data = data;
    wp->num_workers = num_workers;
    wp->do_func = do_func;
    wp->free_func = free_func;
    wp->queue = q;

    for (i = 0; i < num_workers; i++) {
        ASSERT_ALWAYS(pthread_create(&wp->workers[i],
                                     NULL,
                                     _workpool_worker,
                                     (void*)wp) != 0);
    }

done:
    return wp;
}

void workpool_pause(workpool_t *wp)
{
    if (wp) {
        _workpool_set_action(wp, WORKPOOL_ACTION_PAUSE);
    }
}

void workpool_resume(workpool_t *wp)
{
    if (wp) {
        _workpool_set_action(wp, WORKPOOL_ACTION_RUN);
    }
}

void workpool_drain(workpool_t *wp)
{
    if (wp) {
        _workpool_signal_stop(wp, WORKPOOL_ACTION_DRAIN);
    }
}

void workpool_stop(workpool_t *wp)
{
    if (wp) {
        _workpool_signal_stop(wp, WORKPOOL_ACTION_STOP);
    }
}

void workpool_destroy(workpool_t *wp)
{
    if (wp) {
        free(wp->workers);
        wp->workers = NULL;
        free(wp);
        wp = NULL;
    }
}
