/* Q10.c */

#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

int child( char c, int lo, int hi );
int parent( int n );


int main( int argc, char * argv[] )
{
  if ( argc != 3 ) { fprintf( stderr, "USAGE: %s <n> <c>\n", argv[0] ); return EXIT_FAILURE; }

  int n = atoi( argv[1] );
  if ( n < 1 ) { fprintf( stderr, "USAGE: %s <n> <c>\n", argv[0] ); return EXIT_FAILURE; }

  char c = argv[2][0];

  struct stat fileinfo;
  int rc = stat( "data.txt", &fileinfo );
  if ( rc == -1 ) { perror( "stat() failed" ); return EXIT_FAILURE; }
  int bytes = fileinfo.st_size;
  int interval = bytes / n - 1;

  int lo = 0, hi = interval;

  for ( int i = 0 ; i < n ; i++ )
  {
    pid_t pid = fork();
    if ( pid == -1 ) { perror( "fork() failed" ); return EXIT_FAILURE; }

    if ( pid == 0 )
    {
      int rc = child( c, lo, i == n - 1 ? bytes - 1 : hi );
      exit( rc );
    }

    lo = hi + 1;
    hi = lo + interval;
  }

  return parent( n );
}


int child( char c, int lo, int hi )
{
  int fd = open( "data.txt", O_RDONLY );
  if ( fd == -1 ) { perror( "PARENT: open() failed" ); return EXIT_FAILURE; }

  int rc = lseek( fd, lo, SEEK_SET );
  if ( rc == -1 ) { perror( "lseek() failed" ); abort(); }

  int chunk = hi - lo + 1;
  char * buffer = malloc( chunk );
  rc = read( fd, buffer, chunk );

  int count = 0;
  for ( int i = 0 ; i < chunk ; i++ ) if ( buffer[i] == c ) count++;
  free( buffer );

  printf( "Counted %d occurrence%s of '%c' in byte range %d-%d\n", count, count == 1 ? "" : "s", c, lo, hi );
  return EXIT_SUCCESS;
}


int parent( int n )
{
  while ( n > 0 )
  {
    waitpid( -1, NULL, 0 );   /* BLOCKING CALL */
    n--;
  }

  return EXIT_SUCCESS;
}
