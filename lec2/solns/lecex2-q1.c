/* lecex2-q1.c */

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
    printf( "CHILD: happy birthday to me!\n" );
    printf( "CHILD: i'm bored....self-terminating....good-bye!\n" );
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

    printf( "PARENT: child process terminated successfully.\n" );
    printf( "PARENT: sigh, i'm gonna miss that little child process.\n" );
  }

  return EXIT_SUCCESS;
}
