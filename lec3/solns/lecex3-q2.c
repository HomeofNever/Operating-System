/* lecex3-q2.c */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>

#define BUFSIZE 256

void * copy_file( void * arg );

int main( int argc, char * argv[] )
{
  pthread_t * tid = calloc( argc - 1, sizeof( pthread_t ) );

  int i, rc, total_bytes_copied = 0;
  int * bytes_copied_p;

  for ( i = 1 ; i < argc ; i++ )
  {
    printf( "MAIN: Creating thread to copy \"%s\"\n", argv[i] );
    rc = pthread_create( tid + i - 1, NULL, copy_file, argv[i] );
    if ( rc != 0 )
    {
      fprintf( stderr, "MAIN: Could not create thread %d: %s\n",
               i, strerror( rc ) );
    }
  }

  for ( i = 0 ; i < argc - 1 ; i++ )
  {
    rc = pthread_join( *(tid + i), (void **)&( bytes_copied_p ) );
    if ( rc != 0 )
    {
      fprintf( stderr, "MAIN: No thread %d to join: %s\n",
               i, strerror( rc ) );
    }
    else
    {
      printf( "MAIN: Thread completed copying %d byte%s for \"%s\"\n",
              *bytes_copied_p, *bytes_copied_p == 1 ? "" : "s", argv[i] );
      total_bytes_copied += *bytes_copied_p;
      free( bytes_copied_p );
    }
  }

  free( tid );

  printf( "MAIN: Successfully copied %d byte%s via %d child thread%s\n",
          total_bytes_copied, total_bytes_copied == 1 ? "" : "s",
          argc - 1, argc == 2 ? "" : "s" );

  return EXIT_SUCCESS;
}
