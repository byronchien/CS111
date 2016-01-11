#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>

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

      c = getopt_long (argc, argv, "", &option_index);

      if (c == -1)
	break;

      switch(c)
	{
	case 'r':
	  puts ("--rdonly option\n");
	  break;
	case 'w':
	  puts ("--wronly option\n");
	  break;
	case 'c':
	  puts ("--command option\n");
	  break;
	case 'v':
	  puts ("--verbose option\n");
	  break;
	default:
	  break;
	}
    }
  return 0;

}
