/* lecex3-q1.c */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <ctype.h>
#include <sys/ipc.h>
#include <sys/shm.h>

int lecex3_q1_child( int pipefd )
{
  int rc = EXIT_SUCCESS;

  int d[2];
  int bytes_read = read( pipefd, &d, 2 * sizeof( int ) );
  if ( bytes_read == -1 ) { perror( "read() failed" ); rc = EXIT_FAILURE; }
  if ( bytes_read != 2 * sizeof( int ) ) { fprintf( stderr, "did not read() %ld bytes as expected", 2 * sizeof( int ) ); rc = EXIT_FAILURE; }
  close( pipefd );

  int shmid = shmget( d[0], 0, 0 );
  if ( shmid == -1 ) { perror( "shmget() failed" ); return EXIT_FAILURE; }

  char * data = shmat( shmid, NULL, 0 );
  if ( data == (char *)-1 ) { perror( "shmat() failed" ); rc = EXIT_FAILURE; }

  for ( int i = 0 ; i < d[1] ; i++ ) data[i] = toupper( data[i] );

  rc = shmdt( data );
  if ( rc == -1 ) { perror( "shmdt() failed" ); rc = EXIT_FAILURE; }

  return rc;
}
