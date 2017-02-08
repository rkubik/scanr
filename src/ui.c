#include <stdio.h>
#include <string.h>
#include <stdlib.h>

void ui_print_help()
{
  printf("USAGE: ");
  printf("scanr [options] <hostname|ip4|ipv6>\n");
  printf("-----------------------------------\n");
  printf("OPTIONS:\n");
  printf("-o\tPort connection timeout (default: 30 sec)\n");
  printf("-i\tPort analysis\n");
  printf("-p\tPort number\n");
  printf("-l\tPort range low (default: 1)\n");
  printf("-u\tPort range high (default: 65535)\n");
  printf("-t\tNumber of threads (default: 2)\n");
  printf("-v\tPrint software version\n");
  printf("-h\t Print help\n");


}
