/* fork.c */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

int main()
{
  pid_t pid;    /* process id (pid) -- unsigned short */
  printf( "PARENT: okay, start here.\n");

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
    int status;
    pid_t child_pid; 

    child_pid = waitpid( pid, &status, 0 ); 
    if ( WIFSIGNALED( status ) ) {
      printf( "PARENT: ...abnormally (killed by a signal)\n" );
    }
    else /* if ( WEXITED( status ) ) */
    {
        printf( "PARENT: child process terminated successfully.\n");
        printf( "PARENT: sigh, i'm gonna miss that little child process\n");
    }
  }
  else if ( pid == 0 )
  {
    printf( "CHILD: happy birthday to me!\n");
    printf( "CHILD: i'm bored....self-terminating....good-bye!\n");
  }

  return EXIT_SUCCESS;
}