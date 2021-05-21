/* fork-with-waitpid.c */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

int main()
{
  pid_t p;    /* process id (pid) */

  /* create a new (child) process */
  p = fork();

  /* fork() will attempt to create a new process by
      duplicating/copying the existing running process */

  if ( p == -1 )
  {
    perror( "fork() failed" );
    return EXIT_FAILURE;
  }

  if ( p == 0 )
  {
    printf( "CHILD: happy birthday to me!\n" );

#if 0
    double * q = NULL;
    *q = 1.234;   /* Seg fault -- SIGSEGV */
#endif

#if 0
    sleep( 10 );
#endif

    return 47;  /* 0-255 valid range */
/*    return EXIT_FAILURE; */
  }
  else
  {
    printf( "PARENT: my child process id is %d\n", p );

#if 0
    sleep( 10 );
#endif

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