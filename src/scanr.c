#include "scanr/scan.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

int main(int argc, char **argv)
{
    int i, sock, start, end;
    char hostname[100];

    printf("Enter hostname or IP:");
    if(fgets(hostname,99,stdin) == NULL){
      printf("Improper input given. Exiting...\r\n");
      exit(1);
    }

    printf("\n");

    printf("Enter start port number: ");
    scanf("%d", &start);

    printf("\n");

    printf("Enter end port number: ");
    scanf("%d", &end);

    for(i = start;  i <= end; i++) {
        scanr_port_result_t result = {0};

        printf("Scanning port %i: ", i);

        if (scanr_scan_port(hostname, i, &result) != 0) {
            printf("ERROR\n");
        } else {
            switch (result.state) {
                case SCANR_PORT_CLOSED:
                    printf("CLOSED\n");
                    break;
                case SCANR_PORT_OPEN:
                    printf("OPEN\n");
                    break;
                default:
                    printf("UNKNOWN\n");
                    break;
            }
        }
    }

    return 0;
}
