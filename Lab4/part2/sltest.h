#include <getopt.h>

static struct option long_options[] = {
  {"iter=", required_argument, 0, 'i'},
  {"threads=", required_argument, 0, 't'},
  {"yields=", required_argument, 0, 'y'},
  {"sync=", required_argument, 0, 's'},
  {0,0,0,0}
};
