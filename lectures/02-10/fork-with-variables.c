/* fork-with-variables.c */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

/* TO DO: what is the EXACT output of this code?
    -- show all possible outputs using a diagram */

int main()
{
  int x = 5;
  int * y = &x;
  int * z = calloc( 20, sizeof( int ) );


  pid_t p;    /* process id (pid) */

  /* create a new (child) process */
  p = fork();

  /* fork() will attempt to create a new process by
      duplicating/copying the existing running process */

  /* this includes all statically and dynamically allocated
      variables --- x, y, z, p */

  if ( p == -1 )
  {
    perror( "fork() failed" );
    return EXIT_FAILURE;
  }

  if ( p == 0 )
  {
    printf( "CHILD: happy birthday to me!\n" );

    x += 100;
    printf( "CHILD: x is now %d and *y is now %d\n", x, *y );
    z[9] = 47;
    printf( "CHILD: z[9] is %d\n", z[9] );

    return 47;  /* 0-255 valid range */
  }
  else
  {
    printf( "PARENT: my child process id is %d\n", p );

    x += 12;
    printf( "PARENT: x is now %d and *y is now %d\n", x, *y );
    printf( "CHILD: z[9] is %d\n", z[9] );

    int status;
    pid_t child_pid;

    child_pid = waitpid( p, &status, 0 );    /* BLOCKED */

    printf( "PARENT: child process %d terminated...\n", child_pid );

    if ( WIFSIGNALED( status ) )   /* child process was terminated   */
    {                              /*  by a signal (e.g., seg fault) */
      printf( "PARENT: ...abnormally (killed by a signal)\n" );
    }
    else /* if ( WIFEXITED( status ) ) */
    {
      int exit_status = WEXITSTATUS( status );
      printf( "PARENT: ...successfully with exit status %d\n",
              exit_status );
    }

    x += 12;
    printf( "PARENT: x is now %d and *y is now %d\n", x, *y );
    printf( "CHILD: z[9] is %d\n", z[9] );
  }

  return EXIT_SUCCESS;
}

/* POSSIBLE OUTPUTS: only the first two lines of output could be interleaved

PARENT: my child process id is 31223
CHILD: happy birthday to me!
PARENT: child process 31223 terminated...
PARENT: ...successfully with exit status 0

CHILD: happy birthday to me!
PARENT: my child process id is 31223
PARENT: child process 31223 terminated...
PARENT: ...successfully with exit status 0


  To only run the C preprocessor, you can use gcc:

  bash$ gcc -E *.c > tmp.out

 */