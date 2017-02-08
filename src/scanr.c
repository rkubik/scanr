#include "scanr/scan.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <config.h>
#include <getopt.h>
#include<ctype.h>
#define INVALID_PORT "Invalid port number, please supply a number 1-65535.\n"
void ui_print_help();
static void handle_input(int argc, char **argv);
static bool validate_port_number(int number);
int main(int argc, char **argv)
{
    handle_input(argc,argv);
    return(EXIT_SUCCESS);
}

/**
 * @brief Fires appropriate module based upon user input options
 *
 * @param[in] argc - number of arguments passed into program
 * @param[in] argv - Arguments passed into program
 * @param[out] TODO(endian) - do we need an out?
 */
static void handle_input(int argc, char **argv)
{
  int port_number = 0;
  int t_flag = 0;
  int i_flag = 0;
  int p_flag = 0;
  int l_flag = 0;
  int u_flag = 0;
  int o_flag = 0;
  int v_flag = 0;
  int h_flag = 0;

  char *cvalue = NULL;
  int index;
  int c;

  int opterr = 0;

  while ((c = getopt (argc, argv, "t:ip:l:u:o:vh")) != -1)
  {
    switch (c)
      {
      case 't':
        t_flag = 1;
        int threads_count = atoi(optarg);
        /*TODO: Add validation*/
        break;
      case 'i':
        i_flag = 1;
        break;
      case 'p':
        p_flag = 1;
        /*get the argument supplied to p*/
        port_number = atoi(optarg);
        if(!validate_port_number(port_number)){
          printf(INVALID_PORT);
          return;
        }
        break;
      case 'l':
        l_flag = 1;
        int port_number_lower = atoi(optarg);
        if(!validate_port_number(port_number_lower)){
          printf(INVALID_PORT);
          return;
        }
        break;
      case 'u':
        u_flag = 1;
        int port_number_upper = atoi(optarg);
        if(!validate_port_number(port_number_upper)){
          printf(INVALID_PORT);
          return;
        }
        break;
      case 'o':
        o_flag = 1;
        int conn_timeout_seconds = atoi(optarg);
        /*TODO: Add validation*/
        break;
      case 'v':
        printf("scanr %s\n", SCANR_VERSION);
        printf("License MIT: The MIT License <https://opensource.org/licenses/MIT>\n");
        exit(1);
        break;
      case '?':
        if (optopt == 'o' || optopt == 'p'
         || optopt == 'l' || optopt == 'u'
         || optopt == 't'){
          fprintf (stderr, "Option -%c requires an argument.\n", optopt);
        }
        else if (isprint (optopt)){
          fprintf (stderr, "Unknown option `-%c'.\n", optopt);
        }
        else{
          fprintf (stderr,
                   "Unknown option character `\\x%x'.\n",
                   optopt);
        }

      case 'h':
      default:
          ui_print_help();
          exit(1);
          break;
      }
    }
    if (argv[optind] == NULL) {
    printf("Missing target\n");
    exit(1);
  }
  if(p_flag == 1){
    scanr_port_result_t result;
    scanr_scan_port(argv[optind],port_number,&result);
  }
      exit(1);
}

static bool validate_port_number(int number)
{
  return (number > 65535 || number < 1) ? true : false;
}
