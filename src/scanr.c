#include "scanr/scan.h"
#include "util/time.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

static int _port_result_handler(const scanr_port_result_t *result, void *data)
{
    printf("%d\t%s\t%.2f\n",
           result->port,
           scanr_state_string(result->state),
           (double) timespec_milliseconds(&result->conn_latency)/MS_IN_S); 
    return 0;
}

int main(int argc, char **argv)
{
    int ret = 0;
    int sock, start, end;
    char *hostname;

    if (argc != 4) {
        fprintf(stderr, "Usage: %s <hostname> <port start> <port end>\n",
                argv[0]);
        goto done;
    }

    hostname = argv[1];
    start = atoi(argv[2]);
    end = atoi(argv[3]);

    printf("PORT\tSTATE\tLATENCY\n");

    if (scanr_scan_port_range(hostname,
                              start,
                              end,
                              2,
                              _port_result_handler,
                              NULL) != 0) {
        fprintf(stderr, "An error occurred!\n");
        goto done;
    }

    ret = 0;
done:
    return ret;
}
