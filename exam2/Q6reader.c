/* Q6reader.c */

/* Attaches to the shared memory segment created by Q6a.c and
 *  waits for data to be available.  Once data is available, the
 *   data is downcased and displayed.
 *
 * The protocol will be implemented in the first byte of the shared
 *  memory segment as follows:
 *
 *   'R' -- write mode in which this process is reading data
 *
 *   'W' -- read mode in which the other process is writing data
 *
 *   'D' -- detach mode in which both processes should detach
 *           from the shared memory segment
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <ctype.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/types.h>
#include <sys/wait.h>

/* This constant defines the shared memory segment key
 *  such that multiple processes can attach to this segment */
#define SHM_SHARED_KEY 8192

/* This constant defines the size of the shared memory segment */
#define SHM_SIZE 128

int main()
{
  key_t key = SHM_SHARED_KEY;
  int shmid = shmget( key, SHM_SIZE, 0 );
  if ( shmid == -1 ) { perror( "shmget() failed" ); return EXIT_FAILURE; }

  /* attach to the shared memory segment */
  char * data = shmat( shmid, NULL, 0 );
  if ( data == (char *)-1 ) { perror( "shmat() failed" ); return EXIT_FAILURE; }

  do
  {
    while ( data[0] == 'W' ) /* busy wait */ usleep( 100 );

    if ( data[0] == 'R' )
    {
      for ( char * ptr = data + 1 ; *ptr ; ptr++ ) *ptr = tolower( *ptr );
      printf( "DATA: \"%s\"\n", data + 1 );
      data[0] = 'W';
    }
  }
  while ( data[0] != 'D' );

  /* detach from the shared memory segment */
  int rc = shmdt( data );
  if ( rc == -1 ) { perror( "shmdt() failed" ); exit( EXIT_FAILURE ); }

  return EXIT_SUCCESS;
}
