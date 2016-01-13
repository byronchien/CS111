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
	  // printf("%s, %s\n", optarg, argv[optind++]);

	  // optarg is the argument immediately following the option
	  // argv[optind] points to the next argument
	  
	  // check if argv[optind] begins with -- or is equal to null to detect end

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

	  char * newFileDs[3];

	  optind--; // make optind == optarg for consistancy
	  for (int i = 0; i < 3; i++){
	    if (argv[optind] == NULL) {
	      perror ("Error: Not enough file descriptors.\n");
	      exit(0);
	    }
	    printf("%s\n", argv[optind]);

	    newFileDs[i] = argv[optind];
	    optind++;
	  }

	  /* Collect arguments for cmd.
	     Stop conditions:
	     - If you see a '--' (double hyphen)
	     - Hit a NULL byte */
	  int n_args = 0;
	  int t_optind = optind;
	  while(1) {
	    if (argv[t_optind] == NULL) break;
	    if (argv[t_optind][0] == '-' && argv[t_optind][1] == '-') break;
	    n_args++;
	    t_optind++;
	  }
	  printf("Number of args: %d\n", n_args);

	  char **args = malloc(sizeof(char*) * (n_args + 1));
	  for (int i = 0; i < n_args; i++) {
	    args[i] = argv[optind];
	    optind++;
	  }
	  args[n_args] = NULL;
	  for (int i = 0; i < n_args; i++) {
	    printf("%s\n", args[i]);
	  }
	  
	  pid_t pid = fork();

	  if (pid == 0){
	    puts ("this is the child process\n");

	    // copy target file descriptors using a loop through argv
	    // use dup2 to redirect

	    // call execvp and pass in the rest of the --command arguments
	    // execvp (program name, argv[])
	    exit(0);
	  }
	  else {
	    // this is the parent process
	    break;
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
