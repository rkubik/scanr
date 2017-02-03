#ifndef SCANR_NET_H
#define SCANR_NET_H

#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>

/**
 * @brief Convert a hostname/IPv4/IPv6 string to sockaddr.
 *
 * @param[in] hostname - Hostname, IPv4, or IPv6 string
 * @param[in] port - Port number
 * @param[out] ai - Pointer to a addrinfo structure
 *
 * @return 0 on success, -1 on error
 */
int hostname_port_to_addrinfo(const char *hostname,
                              int port,
                              struct addrinfo **ai);

#endif
