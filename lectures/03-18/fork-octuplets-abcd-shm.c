/* fork-octuplets-abcd-shm.c */

#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>
#include <errno.h>
#include <time.h>
#include <string.h>
#include <ctype.h>
#include <sys/ipc.h>
#include <sys/shm.h>


/* This constant defines the shared memory segment that
   multiple processes can use to attach to a shared memory
   segment (which is created below) */
#define SHM_SHARED_KEY 8192

#define CHILDREN 8


/* function prototypes */
int child( int j, int k, char * data, int shmid );
void parent( int children, char * data, int shmid );


int main()
{
  /* create the shared memory segment */
  key_t key = SHM_SHARED_KEY;
  int shmid = shmget( key, 1024, IPC_CREAT | 0660 );

  if ( shmid == -1 )
  {
    perror( "shmget() failed" );
    return EXIT_FAILURE;
  }

  /* attach to the shared memory segment */
  char * data = shmat( shmid, NULL, 0 );

  if ( data == (char *)-1 )
  {
    perror( "shmat() failed" );
    return EXIT_FAILURE;
  }

  /* since we attach to the shared memory segment here
      (i.e., before the fork() call), the child processes
       do not need to call shmat() */


  /*             j  k
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
      int rc = child( j, k, data, shmid );
      exit( rc );        /* be sure to exit the child process */
    }

    j += 4;
    k += 4;
  }

  parent( children, data, shmid );

  return EXIT_SUCCESS;
}


/* each child process converts its range data[j] to data[k] */
/*  to lowercase, sleeps for t seconds, displays the entire */
/*   data string when it wakes up, then return t as exit status */
int child( int j, int k, char * data, int shmid )
{
  srand( time( NULL ) * getpid() );

  int t = 10 + ( rand() % 21 );  /* [10,30] */

  printf( "CHILD %d: I'm gonna nap for %d seconds\n", getpid(), t );
  sleep( t );

  /* downcase the substring [j,k] of the shared memory segment */
  while ( j <= k )
  {
    data[j] = tolower( data[j] );
    j++;
  }

  printf( "CHILD %d: I'm awake: \"%s\"\n", getpid(), data );

  /* child process detaches from the shared memory segment */
  int rc = shmdt( data );

  if ( rc == -1 )
  {
    perror( "shmdt() failed" );
    return EXIT_FAILURE;
  }

  return t;
}


void parent( int children, char * data, int shmid )
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

  /* parent process detaches from the shared memory segment */
  int rc = shmdt( data );

  if ( rc == -1 )
  {
    perror( "shmdt() failed" );
    exit( EXIT_FAILURE );
  }

#if 0
  /* parent process removes the shared memory segment */
  if ( shmctl( shmid, IPC_RMID, 0 ) == -1 )
  {
    perror( "shmctl() failed" );
    exit( EXIT_FAILURE );
  }
#endif

  /* TO DO: try commenting out the waitpid() calls etc. and
             see what happens with the shared memory segment */

}

