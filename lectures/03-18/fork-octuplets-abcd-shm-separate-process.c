/* fork-octuplets-abcd-shm-separate-process.c */

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

/* This constant defines the shared memory segment such that
   multiple processes can attach to this segment */
#define SHM_SHARED_KEY 8192

int main()
{
  /* get the shared memory segment ID */
  key_t key = SHM_SHARED_KEY;
  int shmid = shmget( key, 1024, 0 );

  if ( shmid == -1 )
  {
    perror( "shmget() failed" );
    return EXIT_FAILURE;
  }

  printf( "Found shared memory segment with id %d\n", shmid );

  /* attach to the shared memory segment */
  char * data = shmat( shmid, NULL, 0 );
  if ( data == (char *)-1 )
  {
    perror( "shmat() failed" );
    return EXIT_FAILURE;
  }

  printf( "Shared memory: \"%s\"\n", data );

  /* detach from the shared memory segment */
  int rc = shmdt( data );

  if ( rc == -1 )
  {
    perror( "shmdt() failed" );
    exit( EXIT_FAILURE );
  }

  return EXIT_SUCCESS;
}
