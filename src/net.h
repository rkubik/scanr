#ifndef SCANR_NET_H
#define SCANR_NET_H

/**
 * @brief Clone a addrinfo structure.
 *
 * @param[in] ai - Pointer to a addrinfo structure
 *
 * @return pointer to a newly allocated addrinfo structure. Must be free'd with
 *         freeaddrinfo.
 */
struct addrinfo *getaddrinfo_clone(struct addrinfo *ai);

/**
 * @brief Convert a hostname/IPv4/IPv6 string to sockaddr.
 *
 * @param[in] hostname - Hostname, IPv4, or IPv6 string
 * @param[out] ai - Pointer to a addrinfo structure
 *
 * @return 0 on success, -1 on error
 */
int hostname_to_addrinfo(const char *hostname, struct addrinfo **ai);

/**
 * @brief Set the port in a addrinfo structure. Only IPv4 and IPv6 families
 *        are currently supported.
 *
 * @param[in,out] ai - Pointer to addrinfo structure
 * @param[in] port - Port to set
 */
void addrinfo_set_port(struct addrinfo *ai, int port);

#endif
