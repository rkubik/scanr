#include "scanr/scan.h"

#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <ctype.h>
#include <netdb.h>
#include <unistd.h>
#include <string.h>

/**
 * @brief Make a connection to a socket with a given sockaddr.
 *
 * @param[in] sock - Network socket
 * @param[in] sa - Sock address
 * @param[out] result - Port scan results
 */
static void _scan_sockaddr(int sock,
                           struct sockaddr_in *sa, 
                           scanr_port_result_t *result)
{
    if (connect(sock, (struct sockaddr*)sa, sizeof(*sa)) != 0) {
        result->state = SCANR_PORT_CLOSED;
    } else {
        result->state = SCANR_PORT_OPEN;
    }
}

/**
 * @brief Convert a hostname/ipv4 string to sockaddr.
 *
 * @param[in] hostname - Hostname or ipv4 string
 * @param[out] sa - sockaddr
 *
 * @return 0 on success, -1 on error
 */
static int _hostname_to_sockaddr(const char *hostname, struct sockaddr_in *sa)
{
    int ret = -1;
    struct hostent *host;

    if (!hostname || !sa) {
        goto done;
    }

    if (isdigit(hostname[0])) {
        sa->sin_addr.s_addr = inet_addr(hostname);
    } else if ((host = gethostbyname(hostname)) != 0) {
        strncpy((char*)&sa->sin_addr, (char*)host->h_addr, sizeof(sa->sin_addr));
    } else {
        goto done;
    }

    ret = 0;
done:
    return ret;
}

int scanr_scan_port(const char *hostname, int port, scanr_port_result_t *result)
{
    int ret = -1;
    int sock;
    struct sockaddr_in sa = {0};    

    if (!result) {
        goto done;
    }

    sa.sin_port = htons(port);
    sa.sin_family = AF_INET;

    if (_hostname_to_sockaddr(hostname, &sa) != 0) {
        goto done;
    }

    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock == -1) {
        goto done;
    }

    _scan_sockaddr(sock, &sa, result);
    
    close(sock);

    ret = 0;
done:
    return ret;
}

int scanr_scan_port_range(const char *hostname,
                          int port_from, int port_to,
                          scanr_port_result_cb callback,
                          void *data)
{
    int ret = -1;
    int i;
    int sock;
    struct sockaddr_in sa = {0};

    if (!hostname || !callback || port_from > port_to) {
        goto done;
    }

    if (_hostname_to_sockaddr(hostname, &sa) != 0) {
        goto done;
    } 

    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock == -1) {
        goto done;
    }

    sa.sin_family = AF_INET;

    for (i = port_from; i <= port_to; i++) {
        scanr_port_result_t result = {0};

        sa.sin_port = htons(i);

        _scan_sockaddr(sock, &sa, &result);

        if (callback(&result, data) != 0) {
            break;
        }
    }

    close(sock);

    ret = 0;
done:
    return ret;
}
