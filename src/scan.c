#include "scanr/scan.h"
#include "util/net.h"
#include "util/time.h"

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
#include <stdio.h>
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

int scanr_scan_port(const char *hostname,
                    int port,
                    time_t timeout,
                    scanr_port_result_t *result)
{
    int ret = -1;
    struct addrinfo *ai = NULL;

    if (!result) {
        goto done;
    }

    if (hostname_to_addrinfo(hostname, &ai) != 0) {
        goto done;
    }

    addrinfo_set_port(ai, port);

    if (_scan_addrinfo(ai, timeout, result) != 0) {
        goto done;        
    }

    result->port = port;

    ret = 0;
done:
    if (ai) {
        freeaddrinfo(ai);
        ai = NULL;
    }

    return ret;
}

int scanr_scan_port_range(const char *hostname,
                          int port_from, int port_to,
                          time_t timeout,
                          scanr_port_result_cb callback,
                          void *data)
{
    int ret = -1;
    int port;
    struct addrinfo *ai = NULL;

    if (!hostname || !callback || port_from > port_to) {
        goto done;
    }

    if (hostname_to_addrinfo(hostname, &ai) != 0) {
        goto done;
    } 

    for (port = port_from; port <= port_to; port++) {
        scanr_port_result_t result = {
            .port = port
        };

        addrinfo_set_port(ai, port);

        if (_scan_addrinfo(ai, timeout, &result) != 0) {
            break;
        }

        if (callback(&result, data) != 0) {
           break;
        }
    }

    ret = 0;
done:
    if (ai) {
        freeaddrinfo(ai);
        ai = NULL;
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
