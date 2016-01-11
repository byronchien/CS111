#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>

bool verboseflag = false;;

int main (int argc, char **argv) {
  int c;

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
	  break;
	case 'w':
	  if(verboseflag){
	    puts ("--wronly option\n");
	  }
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
	  break;
	}
    }
  return 0;

}
