/* lecex2-q2.c */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

int main()
{
  pid_t p;

  printf( "PARENT: okay, start here.\n" );
  fflush( stdout );

  p = fork();

  if ( p == -1 )
  {
    perror( "fork() failed" );
    return EXIT_FAILURE;
  }

  if ( p == 0 )
  {
    printf( "CHILD A: happy birthday to me!\n" );
    printf( "CHILD A: i'm bored....self-terminating....good-bye!\n" );
  }
  else /* p > 0 */
  {
    int status;
    pid_t child_pid;

    child_pid = waitpid( p, &status, 0 );    /* BLOCKED */

    if ( child_pid == -1 )
    {
      perror( "waitpid() failed" );
      return EXIT_FAILURE;
    }

    p = fork();

    if ( p == -1 )
    {
      perror( "fork() failed" );
      return EXIT_FAILURE;
    }

    if ( p == 0 )
    {
      printf( "CHILD B: and happy birthday to me!\n" );
      printf( "CHILD B: see ya later....self-terminating!\n" );
    }
    else
    {
      child_pid = waitpid( p, &status, 0 );    /* BLOCKED */

      if ( child_pid == -1 )
      {
        perror( "waitpid() failed" );
        return EXIT_FAILURE;
      }

      printf( "PARENT: both child processes terminated successfully.\n" );
      printf( "PARENT: phew, i'm glad they're gone!\n" );
    }
  }

  return EXIT_SUCCESS;
}
