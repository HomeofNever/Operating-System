/* fork.c */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>


int main()
{
  pid_t pid;    /* process id (pid) -- unsigned short */

  /* create a new (child) process */
  pid = fork();

  if ( pid == -1 )
  {
    perror( "fork() failed" );
    return EXIT_FAILURE;
  }


  if ( pid > 0 )
  {
    /* the PID of the child process is returned in the parent */
    printf( "PARENT: my child process id is %d\n", pid );
    printf( "PARENT: my pid is %d\n", getpid() );
  }
  else if ( pid == 0 )
  {
    printf( "CHILD: happy birthday to me!  My pid is %d.\n", getpid() );
    printf( "CHILD: my parent's pid is %d\n", getppid() );
  }

  /* By adding this last line of output, what is the exact output */
  /*  of this code?  Draw a new diagram.                          */
  printf( "My pid is %d and I'm terminating....\n", getpid() );

  return EXIT_SUCCESS;
}

/* OUTPUT POSSIBILITIES:

goldsd@linux:~/s21$ ./a.out
PARENT: my child process id is 737
PARENT: my pid is 736
CHILD: happy birthday to me!  My pid is 737.
CHILD: my parent's pid is 736

goldsd@linux:~/s21$ ./a.out
PARENT: my child process id is 739
CHILD: happy birthday to me!  My pid is 739.
CHILD: my parent's pid is 738
PARENT: my pid is 738

What are all of the possible outputs for this code?

                                   fork()
                                  /      \
                                 /        \
                                /          \
                            <PARENT>     <CHILD>
PARENT: my child process id is 737     CHILD: happy birthday to me!  My pid is 737.
PARENT: my pid is 736                  CHILD: my parent's pid is 736


   (1) lines shown for <PARENT> interleave with lines shown for <CHILD>

   (2) lines shown for <PARENT> appear in the given order; same for <CHILD>


TO DO: write out each possible output given the diagram above





goldsd@linux:~/s21$ ./a.out
PARENT: my child process id is 1168
PARENT: my pid is 1167
CHILD: happy birthday to me!  My pid is 1168.
CHILD: my parent's pid is 1

 When the parent process terminates, the child process is inherited
  by the root node of the process tree, i.e., process with pid 1

 */
