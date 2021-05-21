/* Q9.c */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <ctype.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/wait.h>

#define MAXBUFFER 80

int child( int * p );
int parent( int * p, int m );


int main( int argc, char * argv[] )
{
  if ( argc != 2 ) { fprintf( stderr, "USAGE: %s <m>\n", argv[0] ); return EXIT_FAILURE; }

  int m = atoi( argv[1] );
  if ( m < 1 ) { fprintf( stderr, "USAGE: %s <m>\n", argv[0] ); return EXIT_FAILURE; }

  int p[2];

  int pipe_rc = pipe( p );
  if ( pipe_rc == -1 ) { perror( "pipe() failed" ); return EXIT_FAILURE; }

  pid_t rc = fork();
  if ( rc == -1 ) { perror( "fork() failed" ); return EXIT_FAILURE; }

  if ( rc == 0 )
  {
    rc = child( p );
  }
  else /* rc > 0 */
  {
    rc = parent( p, m );
  }

  return rc;
}


int child( int * p )
{
  close( p[1] );  /* close the write end of the pipe */

  int rc = EXIT_SUCCESS;
  int digits = 0;
  int bytes = 0;

  while ( 1 )
  {
    char buffer[MAXBUFFER];
    int bytes_read = read( p[0], buffer, MAXBUFFER - 1 );  /* BLOCKING CALL */

    if ( bytes_read == -1 ) { perror( "CHILD: read() failed on pipe" ); rc = EXIT_FAILURE; break; }
    else if ( bytes_read == 0 ) { rc = EXIT_SUCCESS; break; }
    else
    {
      bytes += bytes_read;
      buffer[bytes_read] = '\0';
      for ( char * ptr = buffer ; *ptr ; ptr++ )
        if ( isdigit( *ptr ) ) { printf( "%c", *ptr ); digits++; }
    }
  }

  if ( rc == EXIT_SUCCESS )
  {
    printf( "\n(filtered %d digit%s from %d byte%s)\n",
            digits, digits == 1 ? "" : "s",
            bytes, bytes == 1 ? "" : "s" );
  }

  close( p[0] );

  return rc;
}


int parent( int * p, int m )
{
  close( p[0] );  /* close the read end of the pipe */

  int fd = open( "data.txt", O_RDONLY );
  if ( fd == -1 ) { perror( "PARENT: open() failed" ); return EXIT_FAILURE; }

  int rc = EXIT_SUCCESS;

  while ( m > 0 )
  {
    char buffer[MAXBUFFER];

    int bytes_read = read( fd, buffer, MAXBUFFER - 1 );
    if ( bytes_read == -1 ) { perror( "PARENT: read() failed" ); rc = EXIT_FAILURE; break; }
    else if ( bytes_read == 0 ) { rc = EXIT_SUCCESS; break; }

    if ( bytes_read > m ) buffer[m] = '\0'; else buffer[bytes_read] = '\0';

    rc = write( p[1], buffer, strlen( buffer ) );
    if ( rc == -1 ) { perror( "PARENT: write() failed" ); rc = EXIT_FAILURE; break; }
    m -= bytes_read;
  }

  close( fd );
  close( p[1] );
  waitpid( -1, NULL, 0 );

  return rc;
}
