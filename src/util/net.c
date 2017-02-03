#include "util/net.h"

#include <stdlib.h>
#include <string.h>

#define PORT_SIZE 6

int hostname_port_to_addrinfo(const char *hostname,
                              int port,
                              struct addrinfo **ai)
{
    int ret = -1;
    int err;
    struct addrinfo hints = {0};
    struct addrinfo *res = NULL;
    char port_str[PORT_SIZE] = {0};

    if (!hostname || !ai) {
        goto done;
    }

    err = snprintf(port_str, sizeof(port_str), "%d", port);
    if (err < 0 || (size_t) err >= sizeof(port_str)) {
        goto done;
    }

    hints.ai_family = AF_UNSPEC; /* IPv4 and IPV6 */
    hints.ai_socktype = SOCK_STREAM; /* TCP */
    hints.ai_flags = AI_PASSIVE; /* Fill IP */

    /* getaddrinfo will resolve the hostname, IPv4, or IPv6 strings for us */
    if (getaddrinfo(hostname, port_str, &hints, &res) != 0) {
        goto done;
    }

    *ai = res;
    res = (*ai)->ai_next;
    (*ai)->ai_next = NULL;

    ret = 0;
done:
    if (res) {
        freeaddrinfo(res);
        res = NULL;
    }

    return ret;
}
