#include <getopt.h>

// array of options
static struct option long_options[] = {
  {"rdonly", required_argument, 0, 'r'},
  {"wronly", required_argument, 0, 'w'},
  {"rdwr", required_argument, 0, 'd'},
  {"command", required_argument, 0, 'c'},
  {"close", required_argument, 0, 'l'},
  {"verbose", no_argument, 0, 'v'},
  {"pipe", no_argument, 0, 'p'},
  {"append", no_argument, 0, 'a'},
  {"cloexec", no_argument, 0, 'e'},
  {"creat", no_argument, 0, 't'},
  {"directory", no_argument, 0, 'i'},
  {"dsync", no_argument, 0, 's'},
  {"excl", no_argument, 0, 'x'},
  {"nofollow", no_argument, 0, 'o'},
  {"nonblock", no_argument, 0, 'n'},
  {"rsync", no_argument, 0, 'h'},
  {"sync", no_argument, 0, 'y'},
  {"trunc", no_argument, 0, 'k'},
  {0,0,0,0}
};

const mode_t MODE = 0644;
