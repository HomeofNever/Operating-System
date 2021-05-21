/* fork-octuplets-abcd.c */

#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>
#include <errno.h>
#include <time.h>
#include <string.h>
#include <ctype.h>

#define CHILDREN 8

/* global variable */
char data[1024];   /* this also gets copied to each child process */

/* function prototypes */
int child( int j, int k );
void parent( int children );


int main()
{ /*             j  k
                 |  |
                 v  v  */
  strcpy( data, "ABCDEFGHIJKLMNOPQRSTUVWXYZZZZZZ!!!!!" );

  int j = 0;  /* each child process will work on its own [j,k] substring */
  int k = 3;  /* e.g., "ABCD" */


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
      int rc = child( j, k );
      exit( rc );        /* be sure to exit the child process */
    }

    j += 4;
    k += 4;
  }

  parent( children );

  return EXIT_SUCCESS;
}


/* each child process converts its range data[j] to data[k] */
/*  to lowercase, sleeps for t seconds, displays the entire */
/*   data string when it wakes up, then return t as exit status */
int child( int j, int k )
{
  while ( j <= k )
  {
    data[j] = tolower( data[j] );
    j++;
  }

  srand( time( NULL ) * getpid() );

  int t = 10 + ( rand() % 21 );  /* [10,30] */

  printf( "CHILD %d: I'm gonna nap for %d seconds\n", getpid(), t );
  sleep( t );
  printf( "CHILD %d: I'm awake: \"%s\"\n", getpid(), data );

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
  }

  printf( "PARENT: All done!\n" );
  printf( "PARENT: data is \"%s\"\n", data );
}

