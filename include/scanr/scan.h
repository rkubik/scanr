#include <inttypes.h>

typedef enum {
    SCANR_PORT_CLOSED = 0,
    SCANR_PORT_OPEN
} scanr_port_state_t;

typedef struct {
} scanr_port_info_t;

typedef struct {
    scanr_port_state_t state;
    scanr_port_info_t *info;
} scanr_port_result_t;

/**
 * @brief Callback for port scanning results.
 *
 * @param[in] result - Port results
 * @param[in] data - User data
 *
 * @return 0 to continue scanning, non-zero to stop
 */
typedef int(*scanr_port_result_cb)(const scanr_port_result_t *result, void *data);

/**
 * @brief Scan a single port.
 *
 * @param[in] hostname - Hostname/ipv4 to scan
 * @param[in] port - Port to scan
 * @param[out] result - Port results
 *
 * @return 0 on success, -1 on error
 */
int scanr_scan_port(const char *hostname, int port, scanr_port_result_t *result);

/**
 * @brief Scan a port range. Results will be provided in callback.
 *
 * @param[in] hostname - Hostname/ipv4 to scan
 * @param[in] port_from - Starting port
 * @param[in] port_to - Ending port
 * @param[in] callback - Callback to provide port results
 * @param[in] data - User data
 *
 * @return 0 on success, -1 on error
 */
int scanr_scan_port_range(const char *hostname,
                          int port_from, int port_to,
                          scanr_port_result_cb callback,
                          void *data);
