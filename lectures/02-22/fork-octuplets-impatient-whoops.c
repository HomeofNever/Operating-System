/* fork-octuplets-impatient-whoops.c */

/* this inadvertently kills the parent process, too.... */

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
    /* wait until a child process exits */
    child_pid = waitpid( -1, &status, 0 );   /* BLOCKING CALL */

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

    if ( children <= 4 )
    {
      printf( "PARENT: Wake up already!\n" );
      int rc = kill( 0, SIGTERM );   /* also terminates parent process */
      if ( rc == -1 )
      {
        perror( "kill() failed" );
        /* keep going... */
      }
    }
  }

  printf( "PARENT: All done!\n" );
}

