#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <stdbool.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

bool verboseflag = false;;

int main (int argc, char **argv) {
  int c;

  int offset;
  while (1)
    {
      static struct option long_options[] = {
	{"rdonly",required_argument, 0, 'r'},
	{"wronly",required_argument, 0, 'w'},
	{"command",required_argument, 0, 'c'},
	{"verbose",no_argument, 0, 'v'},
	{0,0,0,0}
      };

      int option_index = 0;

      c = getopt_long (argc, argv, "", long_options, &option_index);

      if (c == -1)
	break;
      
      switch(c)
	{
	case 'r':
	  if(verboseflag){
	    puts ("--rdonly option\n");
	  }
	  //  	  printf("%s, %s\n", optarg, argv[optind++]);
	  printf("fd: %d\n", open(optarg, O_RDONLY));
	  break;
	case 'w':
	  if(verboseflag){
	    puts ("--wronly option\n");
	  }
	  printf("fd: %d\n", open(optarg, O_WRONLY));
	  break;
	case 'c':
	  if(verboseflag){
	    puts ("--command option\n");
	  }
	  break;
	case 'v':
	  verboseflag = true;
	  break;
	default:
	  abort();
	}
    }
  return 0;

}
