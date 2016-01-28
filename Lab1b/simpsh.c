#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <stdbool.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <signal.h>

bool verboseflag = false;;

long long strtonum(const char *nptr,long long minval, long long maxval,
		   const char **errstr);

int main (int argc, char **argv) {
  int c;
  int highestFileDescriptor = 2;
  int numberOfFDs = 0;
  int* validFDs = NULL;

  while (1)
    {
      // array of options
      static struct option long_options[] = {
	{"rdonly",required_argument, 0, 'r'},
	{"wronly",required_argument, 0, 'w'},
	{"command",required_argument, 0, 'c'},
	{"close",required_argument, 0, 'l'},
	{"verbose",no_argument, 0, 'v'},
	{"pipe",no_argument, 0, 'p'},
	{"abort",no_argument, 0, 'a'},
	{0,0,0,0}
      };

      int option_index = 0;

      // retrieve the next option; select case based on value returned
      c = getopt_long (argc, argv, "", long_options, &option_index);

      if (c == -1)
	break;
      
      switch(c)
	{
	case 'a':
	  if (verboseflag) {
	    fprintf(stdout,"--abort");
	  }

	  raise(SIGSEGV);
	  
	case 'p':
	  if(verboseflag){
	    fprintf(stdout,"--pipe\n");
	  }

	  int pipeArgs[2];

	  int check = pipe(pipeArgs);

	  if(check < 0) {
	    fprintf(stderr,"error creating pipe");
	    break;
	  }

	  numberOfFDs += 2;
	  validFDs = realloc(validFDs, numberOfFDs*sizeof(int));
	  validFDs[numberOfFDs - 1] = 1;
	  validFDs[numberOfFDs - 2] = 1;
	  highestFileDescriptor += 2;
	  break;
	  
	case 'l':
	  if(verboseflag){
	    fprintf(stdout,"--close %s\n",optarg);
	  }

	  int fd = atoi(optarg);

	  if (fd >= numberOfFDs) {
	    fprintf(stderr,"invalid argument; file descriptor too large");
	  }

	  validFDs[fd] = 0;
	  break;
	case 'r':
	  if(verboseflag){
	    fprintf(stdout,"--rdonly %s\n",optarg);
	  }
	  
	  // optarg is the argument immediately following the option
	  // argv[optind] points to the next argument
	  
	  // fdtemp contains the file descriptor for the argument passed in
	  // optarg points to the argument immediately following the option
	  int fdtemp1 = open(optarg,O_RDONLY);

	  // check to see whether open returned a valid file descriptor
	  // otherwise throw an error
	  if (fdtemp1  == -1)
	    {
	      fprintf(stderr, "invalid argument to --rdonly");
	      break;
	    }


	  //increment the number of file descriptors
	  //reallocate the array of file descriptors for the new index and
	  //set the new element to 1
	  numberOfFDs++;
	  validFDs = realloc(validFDs, numberOfFDs*sizeof(int));
	  validFDs[numberOfFDs - 1] = 1;

	  
	  highestFileDescriptor++;
	  //printf("RDONLY fd: %d\n", fdtemp1);
	  break;

	case 'w':
	  if(verboseflag){
	    fprintf(stdout,"--wronly %s\n",optarg);
	  }

	  // same as for --rdonly
	  int fdtemp2 = open(optarg, O_WRONLY);
	  //printf("WRONLY fd: %d\n", fdtemp2);

	  // same as for --rdonly; checks for a valid file descriptor
	  if (fdtemp2 == -1)
	    {
	      fprintf(stderr, "invalid argument to --wronly");
	      break;
	    }

  	  //increment the number of file descriptors
	  //reallocate the array of file descriptors for the new index and
	  //set the new element to 1
	  numberOfFDs++;
	  validFDs = realloc(validFDs, numberOfFDs*sizeof(int));
	  validFDs[numberOfFDs - 1] = 1;

	  
 	  highestFileDescriptor++;
	  
	  break;

	case 'c':
	  if(verboseflag){
	    fprintf(stdout,"--command");

	    // loop through all the arguments and print them until a
	    // null byte or another long option is encountered

	    // optind is decremented since optind is set to the second
	    // argument initially rather than the first
	    int temp = optind - 1;
	    while(1) {
	      if ((argv[temp] == NULL) || (argv[temp][0]=='-' && argv[temp][1] == '-')) {
		fprintf(stdout,"\n");
		break;
	      }
	      fprintf(stdout," %s",argv[temp]);
	      temp++;
	    }

	  }

	  // array to store the arguments for new file descriptors
	  int newFileDs[3];

	  // optind is decremented since optind is set to the second
	  // argument initially rather than the first
	  optind--;
	  for (int i = 0; i < 3; i++){
	    const char** ERR = NULL;
	    newFileDs[i] = atoi(argv[optind]);
	    
	    if (newFileDs[i] < 0 || newFileDs[i] > highestFileDescriptor)
	      {
		fprintf (stderr,"invalid file descriptor %d\n", newFileDs[i]);
		break;
	      }
	    else if (validFDs[newFileDs[i]] == 0)
	      {
		fprintf (stderr,"error: accessing closed file (descriptor %d)\n",newFileDs[i]+3);
		break;
	      }
	    else {
	      newFileDs[i] += 3;
	      //printf("%d\n",newFileDs[i]);
	    }
	    
	    optind++;
	  }

	  // count the number of arguments for the command to be run
	  // set a temporary variable to the index of the command and
	  // loop until a long option or a null byte is encountered
	  int n_args = 0;
	  int t_optind = optind;
	  while(1) {
	    if (argv[t_optind] == NULL) break;
	    if (argv[t_optind][0] == '-' && argv[t_optind][1] == '-') break;
	    n_args++;
	    t_optind++;
	  }

	  pid_t pid = fork();

	  if (pid == 0){ //child process

	    for (int k = 0; k < 3; k++) {
	      validFDs[newFileDs[k] - 3] = 0;
	    }

	    for (int k = 0; k < numberOfFDs; k++) {
	      if (validFDs[k] != 0) {
		int j = close(k + 3);
		if (j != 0 ) {
		  fprintf(stderr,"error closing file descriptor %d\n",k);
		  abort();
		}
	      }
	    }
	    

	    // dynamically allocate array for the command's arguments;
	    // copy the arguments from optind's position for the number
	    // of arguments counted previously;	add one extra slot for
	    // a null byte at the end
	    char **args = malloc(sizeof(char*) * (n_args+1));
	    for (int i = 0; i < n_args; i++) {
	      args[i] = argv[optind];
	      optind++;
	    }
	    args[n_args] = NULL;

	    // reassign the file descriptors retrieved previously
	    for(int i = 0; i < 3; i++) {
	      dup2(newFileDs[i],i);	      
	      }

	    // execute the command; args[0] contains the file name and
	    // args contains the previously dynamically allocated array
	    int success = execvp(args[0], args);
	    
	    // free the dynamically allocated array of arguments
	    free(args);
	    free(validFDs);

	    exit(0);
	  }
	  else { //parent process
	   
	    break;
	  }
	  break;
	  
	case 'v':
	  verboseflag = true;
	  break;

	case -1:
	  break;

	default:
	  break;
	}
    }

  for (int k = 0; k < numberOfFDs; k++)
    {
      //printf("%d",validFDs[k]);
    }
  free(validFDs);
  return 0;

}
