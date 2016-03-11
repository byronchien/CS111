#include <getopt.h>

static struct option long_options[] = {
  {"iter=", required_argument, 0, 'i'},
  {"threads=", required_argument, 0, 't'},
  {"yields=", required_argument, 0, 'y'},
  {"sync=", required_argument, 0, 's'},
  {"lists=", required_argument, 0, 'l'},
  {"correct", no_argument, 0, 'c'},
  {0,0,0,0}
};
