#include "simpsh.h"
#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <stdbool.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <signal.h>
#include <sys/resource.h>
#include <sys/time.h>

void catch_handler(int signum, siginfo_t *s, void *args) {
  fprintf(stderr, "%d caught\n", signum);
  exit(signum);
}

int main (int argc, char **argv) {
  int c;
  int highestFileDescriptor = 2;
  int numberOfFDs = 0;
  int* validFDs = NULL;
  int oflags = 0;
  
  bool verboseflag = false;
  bool profileFlag = false;
  int exitStatus = 0;
  
  command_info* _commands = NULL;
  int ncmds = 0;

  struct rusage usage;

  while (1)
    {
      int option_index = 0;

      // retrieve the next option; select case based on value returned
      c = getopt_long (argc, argv, "", long_options, &option_index);

      if (c == -1)
	break;
      
      switch(c)
	{
	case 'a':
	  oflags |= O_APPEND;
	  break;
	case 'e':
	  oflags |= O_CLOEXEC;
	  break;
	case 't':
	  oflags |= O_CREAT;
	  break;
	case 'i':
	  oflags |= O_DIRECTORY;
	  break;
	case 's':
	  oflags |= O_DSYNC;
	  break;
	case 'x':
	  oflags |= O_EXCL;
	  break;
	case 'o':
	  oflags |= O_NOFOLLOW;
	  break;
	case 'n':
	  oflags |= O_NONBLOCK;
	  break;
	case 'h':
	  oflags |= O_RSYNC;
	  break;
	case 'y':
	  oflags |= O_SYNC;
	  break;
	case 'k':
	  oflags |= O_TRUNC;
	  break;
        case 'r':
	case 'w':
	case 'd':
	  if (c == 'r') {
	    oflags |= O_RDONLY;
	    if(verboseflag) fprintf(stdout, "--rdonly %s\n", optarg);
	    if(profileFlag) fprintf(stdout, "--rdonly %s\n", optarg);
	  }
	  else if (c == 'w') {
	    oflags |= O_WRONLY;
	    if(verboseflag) fprintf(stdout, "--wronly %s\n", optarg);
	    if(profileFlag) fprintf(stdout, "--wronly %s\n", optarg);
	  }
	  else if (c == 'd') {
	    oflags |= O_RDWR;
	    if(verboseflag) fprintf(stdout, "--rdwr %s\n", optarg);
	    if(profileFlag) fprintf(stdout, "--rdwr %s\n", optarg);
	  }

	  int tempfd = open(optarg, oflags, MODE);
	  if (tempfd == -1) {
	    fprintf(stderr, "Invalid argument for file.\n");
	    exitStatus = 1;
	    break;
	  }

	  numberOfFDs++;
	  validFDs = realloc(validFDs, numberOfFDs * sizeof(int));
	  validFDs[numberOfFDs - 1] = 1;

	  highestFileDescriptor++;
	  oflags = 0;

	  if (profileFlag) {
	    getrusage(RUSAGE_SELF, &usage);
	    struct timeval utime = usage.ru_utime;
	    struct timeval stime = usage.ru_stime;
	    fprintf(stdout, "System time: %d\n", (int)stime.tv_usec);
	    fprintf(stdout, "User time: %d\n\n", (int)utime.tv_usec);
	  }
	  break;

	  
	case 'b':
	  if (verboseflag) {
	    fprintf(stdout,"--abort\n");
	  }

	  if (profileFlag) {
	    getrusage(RUSAGE_SELF, &usage);
	    struct timeval utime = usage.ru_utime;
	    struct timeval stime = usage.ru_stime;
	    fprintf(stdout, "--abort\n");
	    fprintf(stdout, "System time: %d\n", (int)stime.tv_usec);
	    fprintf(stdout, "User time: %d\n\n", (int)utime.tv_usec);
	  }
	  
	  raise(SIGSEGV);
	  
	case 'p':
	  if(verboseflag){
	    fprintf(stdout,"--pipe\n");
	  }

	  int pipeArgs[2];

	  int check = pipe(pipeArgs);

	  if(check < 0) {
	    fprintf(stderr,"Error creating pipe\n");
	    exitStatus = 1;
	    break;
	  }

	  numberOfFDs += 2;
	  validFDs = realloc(validFDs, numberOfFDs*sizeof(int));
	  validFDs[numberOfFDs - 1] = 1;
	  validFDs[numberOfFDs - 2] = 1;
	  highestFileDescriptor += 2;
	  
	  if (profileFlag) {
	    getrusage(RUSAGE_SELF, &usage);
	    struct timeval utime = usage.ru_utime;
	    struct timeval stime = usage.ru_stime;
	    fprintf(stdout, "--pipe\n");
	    fprintf(stdout, "System time: %d\n", (int)stime.tv_usec);
	    fprintf(stdout, "User time: %d\n\n", (int)utime.tv_usec);
	  }

	  break;

	case 'q':
	  if(verboseflag) fprintf(stdout, "--profile\n");
	  profileFlag = true;
	  break;
	  
	case 'l':
	  if(verboseflag){
	    fprintf(stdout,"--close %s\n",optarg);
	  }

	  int fd = atoi(optarg);

	  if (fd >= numberOfFDs) {
	    fprintf(stderr,"invalid argument; file descriptor too large\n");
	    exitStatus = 1;
	  }

	  validFDs[fd] = 0;

	  if (profileFlag) {
	    getrusage(RUSAGE_SELF, &usage);
	    struct timeval utime = usage.ru_utime;
	    struct timeval stime = usage.ru_stime;
	    fprintf(stdout, "--close\n");
	    fprintf(stdout, "System time: %d\n", (int)stime.tv_usec);
	    fprintf(stdout, "User time: %d\n\n", (int)utime.tv_usec);
	  }

	  break;

	  /*  OLD EXMAPLE (left here for comments)
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
	  END OF EXAMPLE */

	case 'c':
	  ncmds++;
	  _commands = realloc(_commands, ncmds*sizeof(command_info));

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
	  bool shouldI = 1;
	  for (int i = 0; i < 3; i++){
	    const char** ERR = NULL;
	    newFileDs[i] = atoi(argv[optind]);

	    if (newFileDs[i] < 0 || newFileDs[i] > highestFileDescriptor - 3)
	      {
		fprintf (stderr,"invalid file descriptor %d\n", newFileDs[i]);
		exitStatus = 1;
		shouldI = 0;
		break;
	      }
	    else if (validFDs[newFileDs[i]] == 0)
	      {
		fprintf (stderr,"error: accessing closed file (descriptor %d)\n",newFileDs[i]+3);
		exitStatus = 1;
		shouldI = 0;
		break;
	      }
	    else {
	      newFileDs[i] += 3;
	      //printf("%d\n",newFileDs[i]);
	    }
	    
	    optind++;
	  }

	  if(!shouldI)
	    break;
	  
	  // count the number of arguments for the command to be run
	  // set a temporary variable to the index of the command and
	  // loop until a long option or a null byte is encountered
	  int n_args = 0;
	  int t_optind = optind;

	  while(shouldI) {
	    if (argv[t_optind] == NULL) break;
	    if (argv[t_optind][0] == '-' && argv[t_optind][1] == '-') break;
	    n_args++;
	    t_optind++;
	  }

	  _commands[ncmds - 1].s_ptr = malloc(sizeof(char*)*n_args);

	  t_optind = optind;
	  int _index = 0;
	  while(1) {
	    if (argv[t_optind] == NULL) break;
	    if (argv[t_optind][0] == '-' && argv[t_optind][1] == '-') break;
	    _commands[ncmds - 1].s_ptr[_index] = argv[t_optind];

	    _index++;
	    t_optind++;
	  }
	  
	  _commands[ncmds - 1].nargs = n_args;
	  
	  pid_t pid = fork();

	  if (pid == 0){ //child process
	    for (int k = 0; k < 3; k++) {
	      validFDs[newFileDs[k] - 3] = 0;
	    }

	    for (int k = 0; k < numberOfFDs; k++) {
	      if (validFDs[k] != 0) {
		int j = close(k + 3);
		if (j != 0 ) {
		  fprintf(stderr,"Error closing file descriptor %d\n",k);
		  exitStatus = 1;
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
	    free(_commands);
	    	    
	    exit(0);
	  }
	  else { //parent process
	    _commands[ncmds - 1].c_pid = pid;
	    if (profileFlag) {
	      getrusage(RUSAGE_SELF, &usage);
	      struct timeval utime = usage.ru_utime;
	      struct timeval stime = usage.ru_stime;
	      fprintf(stdout,"--command");
	      int temp = optind - 3;
	      while(1) {
		if ((argv[temp] == NULL) || (argv[temp][0]=='-' && argv[temp][1] == '-')) {
		  fprintf(stdout,"\n");
		  break;
		}
		fprintf(stdout," %s",argv[temp]);
		temp++;
	      }
	      fprintf(stdout, "System time: %d\n", (int)stime.tv_usec);
	      fprintf(stdout, "User time: %d\n\n", (int)utime.tv_usec);
	    }
	    break;
	  }
	  break;
	  
	case 'v':
	  verboseflag = true;
	  if (profileFlag) {
	    getrusage(RUSAGE_SELF, &usage);
	    struct timeval utime = usage.ru_utime;
	    struct timeval stime = usage.ru_stime;
	    fprintf(stdout, "--verbose\n");
	    fprintf(stdout, "System time: %d\n", (int)stime.tv_usec);
	    fprintf(stdout, "User time: %d\n\n", (int)utime.tv_usec);
	  }
	  
	  break;
	  
	case 'z': // this is --wait
	  if (verboseflag) {
	    fprintf(stdout, "--wait\n");
	  }

	  for (int k =0; k < numberOfFDs;k++)
	    {
	      close(k+3);
	    }
	  
	  for (int k = 0; k < ncmds; k++)
	    {

	      int status = 0;

	      waitpid(_commands[k].c_pid, &status, 0 );

	      if (WIFEXITED(status))
		{
		  fprintf(stdout,"%d ", WEXITSTATUS(status));
		  if(WEXITSTATUS(status) > exitStatus)
		    exitStatus = WEXITSTATUS(status);
		}
	      else
		{
		  fprintf(stdout,"\nDid not exit properly\n");
		  continue;
		}

	      fprintf(stdout,"%s",_commands[k].s_ptr[0]);
	      
	      for (int i = 1; i < _commands[k].nargs; i++)
		{
		  fprintf(stdout," %s",_commands[k].s_ptr[i]);
		}

	      fprintf(stdout,"\n");
	    }

	  if (profileFlag) {
	    getrusage(RUSAGE_SELF, &usage);
	    struct timeval utime = usage.ru_utime;
	    struct timeval stime = usage.ru_stime;
	    fprintf(stdout, "\n--wait\n");
	    fprintf(stdout, "System time: %d\n", (int)stime.tv_usec);
	    fprintf(stdout, "User time: %d\n\n", (int)utime.tv_usec);
	  }

	  break;

	  
	  /*
	    ====================
	    ====Signal Stuff====
	    ====================
	   */

	case 'g': // catch
	  if (verboseflag) {
	    fprintf(stdout, "--catch %d\n", atoi(optarg));
	  }
	  struct sigaction sa;
	  sa.sa_sigaction = catch_handler;
	  int sig = atoi(optarg);
	  if (sig < 0) {
	    fprintf(stderr, "Invalid signal number\n");
	    exitStatus = 1;
	    break;
	  }
	  sigaction(sig, &sa, NULL);

	  if (profileFlag) {
	    getrusage(RUSAGE_CHILDREN, &usage);
	    struct timeval utime = usage.ru_utime;
	    struct timeval stime = usage.ru_stime;
	    fprintf(stdout, "--catch %d\n", atoi(optarg));
	    fprintf(stdout, "System time: %d\n", (int)stime.tv_usec);
	    fprintf(stdout, "User time: %d\n\n", (int)utime.tv_usec);
	  }

	  break;

	case 'f': // ignore
	  if (verboseflag) {
	    fprintf(stdout, "--ignore %d\n", atoi(optarg));
	  }
	  int si = atoi(optarg);
	  if ( si < 0 ) {
	    fprintf(stderr, "Invalid signal number\n");
	    exitStatus = 1;
	    break;
	  }
	  signal(si, SIG_IGN);

	  if (profileFlag) {
	    getrusage(RUSAGE_SELF, &usage);
	    struct timeval utime = usage.ru_utime;
	    struct timeval stime = usage.ru_stime;
	    fprintf(stdout, "--ignore %d\n", atoi(optarg));
	    fprintf(stdout, "System time: %d\n", (int)stime.tv_usec);
	    fprintf(stdout, "User time: %d\n\n", (int)utime.tv_usec);
	  }

	  break;

	case 'j': // default
	  if (verboseflag) {
	    fprintf(stdout, "--ignore %d\n", atoi(optarg));
	  }
	  int s = atoi(optarg);
	  if ( s < 0 ) {
	    fprintf(stderr, "Invalid signal number\n");
	    exitStatus = 1;
	    break;
	  }
	  signal(s, SIG_DFL);

	  if (profileFlag) {
	    getrusage(RUSAGE_SELF, &usage);
	    struct timeval utime = usage.ru_utime;
	    struct timeval stime = usage.ru_stime;
	    fprintf(stdout, "--abort %d\n", atoi(optarg));
	    fprintf(stdout, "System time: %d\n", (int)stime.tv_usec);
	    fprintf(stdout, "User time: %d\n\n", (int)utime.tv_usec);
	  }

	  break;

	case 'u':
	  if (verboseflag) {
	    fprintf(stdout, "--pause\n");
	  }
	  pause();
	  
	  if (profileFlag) {
	    getrusage(RUSAGE_SELF, &usage);
	    struct timeval utime = usage.ru_utime;
	    struct timeval stime = usage.ru_stime;
	    fprintf(stdout, "--pause\n");
	    fprintf(stdout, "System time: %d\n", (int)stime.tv_usec);
	    fprintf(stdout, "User time: %d\n\n", (int)utime.tv_usec);
	  }

	  break;
	  
	case -1:
	  break;

	default:
	  break;
	}
    }
  
  free(validFDs);

  for (int k = 0; k < ncmds; k++)
    {
      free(_commands[k].s_ptr);
    }
  
  free(_commands);
  exit(exitStatus);

}

