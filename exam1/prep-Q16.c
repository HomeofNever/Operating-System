/* prep-Q16.c */

/* see corresponding video, to be posted soon...! */

#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>
#include <errno.h>
#include <time.h>
#include <signal.h>

/* global array of pids */
pid_t * pids = NULL;

/* global size of pids array */
int children = 0;

/* function prototypes */
int child();
int parent();
void sighandler( int sig );


int main( int argc, char * argv[] )
{
  signal( SIGINT, SIG_IGN );

  if ( argc != 2 ) { fprintf( stderr, "USAGE: %s <n>\n", argv[0] ); return EXIT_FAILURE; }

  children = atoi( argv[1] );
  if ( children < 1 ) { fprintf( stderr, "USAGE: %s <n>\n", argv[0] ); return EXIT_FAILURE; }

  pids = calloc( children, sizeof( pid_t ) );
  if ( pids == NULL ) { perror( "calloc() failed" ); return EXIT_FAILURE; }

  for ( int i = 0 ; i < children ; i++ )
  {
    pid_t pid = fork();
    if ( pid == -1 ) { perror( "fork() failed" ); return EXIT_FAILURE; }

    if ( pid == 0 )
    {
      free( pids );      /* free up the global memory in each child process */
      int rc = child( i + 1 );
      exit( rc );        /* be sure to exit the child process */
    }

    pids[i] = pid;  /* or  *(pids+i)=pid;  */
  }

  signal( SIGINT, sighandler );

  int rc = parent();

  free( pids );  /* free up the memory in the parent process */

  return rc;
}


int child( int x )
{
  while ( 1 )
  {
    printf( "PID %d\n", getpid() );
    sleep( x );
  }
  return EXIT_FAILURE;
}

void sighandler( int sig )
{
  for ( int i = 0 ; i < children ; i++ )
  {
    int rc = kill( pids[i], SIGTERM );
    if ( rc == -1 ) { perror( "kill() failed" ); }
  }
}

int parent()
{
  while ( children > 0 )
  {
    int status;  /* exit status from each child process */

    /* wait until a child process exits */
    pid_t child_pid = waitpid( -1, &status, 0 );   /* BLOCKING CALL */

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

  return EXIT_SUCCESS;
}
