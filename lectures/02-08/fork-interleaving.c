/* fork-interleaving.c */

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


  /* delay BOTH parent and child processes -- could also use sleep() */
  int i;
  for ( i = 0 ; i < 100000000 ; i++ )
    ;


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

What are all of the possible outputs for this code?

                                   fork()
                                  /      \
                                 /        \
                                /          \
                            <PARENT>     <CHILD>
PARENT: my child process id is 737     CHILD: happy birthday to me!  My pid is 737.
PARENT: my pid is 736                  CHILD: my parent's pid is 736
My pid is 736 and I'm terminating....  My pid is 737 and I'm terminating....


   (1) lines shown for <PARENT> interleave with lines shown for <CHILD>

   (2) lines shown for <PARENT> appear in the given order; same for <CHILD>


TO DO: write out each possible output given the diagram above

 */
