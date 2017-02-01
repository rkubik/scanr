#include "scanr/scan.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

int main(int argc, char **argv)
{
    int i, sock, start, end;
    char *hostname;

    if (argc != 4) {
        printf("Usage: %s <hostname> <port start> <port end>\n", argv[0]);
        exit(1);
    }

    hostname = argv[1];
    start = atoi(argv[2]);
    end = atoi(argv[3]);

    for(i = start;  i <= end; i++) {
        scanr_port_result_t result = {0};

        printf("[%s] Scanning port %i: ", hostname, i);

        if (scanr_scan_port(hostname, i, 2, &result) != 0) {
            printf("ERROR\n");
        } else {
            switch (result.state) {
                case SCANR_PORT_CLOSED:
                    printf("CLOSED\n");
                    break;
                case SCANR_PORT_OPEN:
                    printf("OPEN\n");
                    break;
                case SCANR_PORT_TIMEOUT:
                    printf("TIMEOUT\n");
                    break;
                default:
                    printf("UNKNOWN\n");
                    break;
            }
        }
    }

    return 0;
}
