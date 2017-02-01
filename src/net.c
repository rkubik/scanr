#include "net.h"

#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <stdlib.h>
#include <string.h>

struct addrinfo *getaddrinfo_clone(struct addrinfo *ai)
{
    struct addrinfo *copy = NULL;

    if (!ai) {
        goto done;
    }

    copy = calloc(1, sizeof(struct addrinfo));
    if (!copy) {
        goto done;
    }

    copy->ai_flags = 0; /* make sure AI_CLONE is off */
    copy->ai_family = ai->ai_family;
    copy->ai_socktype = ai->ai_socktype;
    copy->ai_protocol = ai->ai_protocol;
    copy->ai_addrlen = ai->ai_addrlen;
    copy->ai_addr = malloc(ai->ai_addrlen);
    if (!copy->ai_addr) {
        free(copy);
        copy = NULL;
        goto done;
    }
    memcpy(copy->ai_addr, ai->ai_addr, ai->ai_addrlen);
done:
    return copy;
}

int hostname_to_addrinfo(const char *hostname, struct addrinfo **ai)
{
    int ret = -1;
    struct addrinfo hints = {0};
    struct addrinfo *res = NULL;

    if (!hostname || !ai) {
        goto done;
    }

    hints.ai_family = AF_UNSPEC; /* IPv4 and IPV6 */
    hints.ai_socktype = SOCK_STREAM; /* TCP */
    hints.ai_flags = AI_PASSIVE; /* Fill IP */

    /* getaddrinfo will resolve the hostname, IPv4, or IPv6 strings for us */
    if (getaddrinfo(hostname, NULL, &hints, &res) != 0) {
        goto done;
    }

    /* We only expect one addrinfo structure to be returned */
    *ai = getaddrinfo_clone(res);
    if (!*ai) {
        goto done;
    }

    ret = 0;
done:
    if (res) {
        freeaddrinfo(res);
        res = NULL;
    }

    return ret;
}

void addrinfo_set_port(struct addrinfo *ai, int port)
{
    struct sockaddr_in *in4;
    struct sockaddr_in6 *in6;

    if (ai) {
        switch (ai->ai_family) {
            case AF_INET:
                in4 = (struct sockaddr_in*)ai->ai_addr;
                in4->sin_port = htons(port);
                break;
            case AF_INET6:
                in6 = (struct sockaddr_in6*)ai->ai_addr;
                in6->sin6_port = htons(port);
                break;
            default:
                break;
        }
    }
}
