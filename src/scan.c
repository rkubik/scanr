#include "scanr/scan.h"
#include "util/net.h"
#include "util/time.h"
#include "util/queue.h"
#include "util/workpool.h"

#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <ctype.h>
#include <netdb.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <time.h>
#include <sys/time.h>
#include <sys/select.h>
#include <errno.h>
#include <stdlib.h>

typedef struct {
    int port;
    time_t timeout;
    const char *hostname;
    scanr_port_result_cb result;
} _scanr_port_request_t;

/**
 * @brief Make a connection to a socket with a given sockaddr.
 *
 * @param[in] ai - Pointer to a addrinfo structure
 * @param[in] timeout - Connection timeout (seconds)
 * @param[out] result - Port scan results
 *
 * @return 0 on success, or -1 on error
 */
static int _scan_connect(struct addrinfo *ai,
                         time_t timeout,
                         scanr_port_result_t *result)
{
    int ret = -1;
    int err;
    struct timeval tv;
    fd_set fdset;
    int so_error;
    socklen_t len = sizeof(so_error);
    int sock = -1;

    sock = socket(ai->ai_family, ai->ai_socktype, ai->ai_protocol);
    if (sock == -1) {
        goto done;
    }

    if (fcntl(sock, F_SETFL, O_NONBLOCK) != 0) {
        goto done;
    }

    FD_ZERO(&fdset);
    FD_SET(sock, &fdset);
    tv.tv_sec = timeout;
    tv.tv_usec = 0;

    err = connect(sock, ai->ai_addr, ai->ai_addrlen);
    if (err == 0) {
        ret = 0;
        result->state = SCANR_PORT_OPEN;
        goto done;
    } else if (errno == ECONNREFUSED) {
        ret = 0;
        result->state = SCANR_PORT_CLOSED;
        goto done;
    } else if (errno != EINPROGRESS) {
        goto done;
    }

    err = select(sock+1, NULL, &fdset, NULL, &tv);
    if (err < 0) {
        goto done;
    }

    if (err == 1) {
        if (getsockopt(sock, SOL_SOCKET, SO_ERROR, &so_error, &len) != 0) {
            goto done;
        }

        if (so_error == 0) {
            result->state = SCANR_PORT_OPEN;
        } else {
            result->state = SCANR_PORT_CLOSED;
        }
    } else {
        result->state = SCANR_PORT_TIMEOUT;
    }

    ret = 0;
done:
    if (sock != -1) {
        close(sock);
        sock = -1;
    }

    return ret;
}

static int _scan_addrinfo(struct addrinfo *ai,
                          time_t timeout,
                          scanr_port_result_t *result)
{
    int ret = -1;
    struct timespec tp1 = {0};
    struct timespec tp2 = {0};

    if (clock_gettime(CLOCK_MONOTONIC, &tp1) != 0) {
        goto done;
    }

    if (_scan_connect(ai, timeout, result) != 0) {
        goto done;
    }

    if (clock_gettime(CLOCK_MONOTONIC, &tp2) != 0) {
        goto done;
    }

    timespec_diff(&tp2, &tp1, &result->conn_latency);

    ret = 0;
done:
    return ret;
}

static void _scan_worker(void *item, void *data)
{
    _scanr_port_request_t *pr = item;
    scanr_port_result_t res = {0};
    struct addrinfo *ai = NULL;

    do {
        if (!pr) {
            break;
        }

        res.port = pr->port;

        if (hostname_to_addrinfo(pr->hostname, &ai) != 0) {
            break;
        }

        addrinfo_set_port(ai, pr->port);

        if (_scan_addrinfo(ai, pr->timeout, &res) != 0) {
            break;
        }

        if (pr->result(&res, data) != 0) {
            /* @todo signal to stop scanning */
        }       
    } while (0);

    if (pr) {
        free(pr);
        pr = NULL;
    }

    if (ai) {
        freeaddrinfo(ai);
        ai = NULL;
    }
}

int scanr_scan_port_range(const char *hostname,
                          int port_start,
                          int port_end,
                          time_t timeout,
                          size_t num_workers,
                          scanr_port_result_cb callback,
                          void *data)
{
    int ret = -1;
    queue_t *q = NULL;
    workpool_t *wp;
    int port;
    _scanr_port_request_t *pr;

    if (!hostname || num_workers < 1 || port_start > port_end || !callback) {
        goto done;
    }

    q = queue_create();
    if (!q) {
        goto done;
    }

    wp = workpool_create(_scan_worker,
                         NULL,
                         num_workers,
                         q,
                         data);
    if (!wp) {
        goto done;
    }

    /* Add port structures to queue */
    for (port = port_start; port <= port_end; port++) {
        pr = calloc(1, sizeof(_scanr_port_request_t));
        if (pr) {
            pr->port = port;
            pr->timeout = timeout;
            pr->hostname = hostname;
            pr->result = callback;

            if (queue_push(q, (void*)pr) != 0) {
                /* @todo message */
            }
        } else {
            /* @todo message */
        }
    }

    /* Wait until queue is empty */
    workpool_drain(wp);

    ret = 0;
done:
    if (q) {
        queue_destroy(q);
        q = NULL;
    }

    return ret;
}

const char *scanr_state_string(const scanr_port_state_t state)
{
    const char *str = NULL;

    switch (state) {
        case SCANR_PORT_OPEN:
            str = "OPEN";
            break;
        case SCANR_PORT_CLOSED:
            str = "CLOSED";
            break;
        case SCANR_PORT_TIMEOUT:
            str = "TIMEOUT";
            break;
        default:
            str = "UNKNOWN";
            break;
    }

    return str;
}
