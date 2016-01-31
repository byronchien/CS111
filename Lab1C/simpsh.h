#include <getopt.h>
#include <sys/stat.h>
#include <unistd.h>

// array of options
static struct option long_options[] = {
  {"append", no_argument, 0, 'a'},
  {"abort", no_argument, 0, 'b'},
  {"command", required_argument, 0, 'c'},
  {"rdwr", required_argument, 0, 'd'},
  {"cloexec", no_argument, 0, 'e'},
  {"ignore", required_argument, 0, 'f'},
  {"catch", required_argument, 0, 'g'},
  {"rsync", no_argument, 0, 'h'},
  {"directory", no_argument, 0, 'i'},
  {"default", required_argument, 0, 'j'},
  {"trunc", no_argument, 0, 'k'},
  {"close", required_argument, 0, 'l'},
  {"nonblock", no_argument, 0, 'n'},
  {"nofollow", no_argument, 0, 'o'},
  {"pipe", no_argument, 0, 'p'},
  {"rdonly", required_argument, 0, 'r'},  
  {"dsync", no_argument, 0, 's'},
  {"creat", no_argument, 0, 't'},
  {"pause", no_argument, 0, 'u'},
  {"verbose", no_argument, 0, 'v'},
  {"wronly", required_argument, 0, 'w'},
  {"excl", no_argument, 0, 'x'},
  {"sync", no_argument, 0, 'y'},
  {"wait", no_argument, 0, 'z'},
  {0,0,0,0}
};

struct command_info{
  pid_t c_pid; // pid of corresponding forked process
  char** s_ptr; // starting index of arguments
  int nargs; // number of arguments
};

typedef struct command_info command_info;

const mode_t MODE = 0644;
