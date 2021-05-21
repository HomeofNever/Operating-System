/* fork-octuplets-WNOHANG.c */

#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>
#include <errno.h>
#include <time.h>

#define CHILDREN 8

/* function prototypes */
int child();
void parent( int children );


int main()
{
  int i, children = CHILDREN;

  for ( i = 0 ; i < children ; i++ )
  {
    pid_t pid = fork();

    if ( pid == -1 )
    {
      perror( "fork() failed" );
      return EXIT_FAILURE;
    }

    if ( pid == 0 )
    {
      int rc = child();
      exit( rc );        /* be sure to exit the child process */
    }
  }

  parent( children );

  return EXIT_SUCCESS;
}


/* each child process will sleep for t second,    */
/*  then return t as exit status when it wakes up */
int child()
{
  srand( time( NULL ) * getpid() );

  int t = 10 + ( rand() % 21 );  /* [10,30] */

  printf( "CHILD %d: I'm gonna nap for %d seconds\n", getpid(), t );
  sleep( t );
  printf( "CHILD %d: I'm awake!\n", getpid() );

  return t;
}


void parent( int children )
{
  int status;  /* exit status from each child process */

  pid_t child_pid;

  printf( "PARENT: I'm waiting for my children to wake up\n" );

  while ( children > 0 )
  {

    /* check to see if a child process has terminated... */
    child_pid = waitpid( -1, &status, WNOHANG );  /* NON-BLOCKING CALL */

    if ( child_pid == 0 )
    {
      printf( "PARENT: no children woke up...try again in 1 second\n" );
      sleep( 1 );
      continue;
    }


    children--;

    printf( "PARENT: child process %d terminated...", child_pid );

    if ( WIFSIGNALED( status ) )
    {
      printf( "abnormally\n" );
    }
    else /* if ( WIFEXITED( status ) ) */
    {
      int rc = WEXITSTATUS( status );
      printf( "successfully with exit status %d\n", rc );
    }

    printf( "PARENT: %d children to go....\n", children );
  }

  printf( "PARENT: All done!\n" );
}

