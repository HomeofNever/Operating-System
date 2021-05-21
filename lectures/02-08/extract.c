/* extract.c */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

int main( int argc, char * argv[] )
{
  /* TO DO: check argc to be sure we have the correct number of arguments! */

  int fd = open( argv[1], O_RDONLY );

  if ( fd == -1 )
  {
    perror( "open() failed" );
    return EXIT_FAILURE;
  }


  /* advance to the 7th byte in the file */
  int rc = lseek( fd, 6, SEEK_SET );

  if ( rc == -1 )
  {
    perror( "lseek() failed" );
    return EXIT_FAILURE;
  }


  char buffer;
  rc = read( fd, &buffer, 1 );
  /* TO DO: check the return value from read() here and below for error! */

  while ( rc )  /* while ( rc == 1 )  <=== why is this not ideal? */
  {
    printf( "%c", buffer );
    rc = lseek( fd, 6, SEEK_CUR );
    rc = read( fd, &buffer, 1 );
  }

  /* how do we know the above loop will stop? */

  printf( "\n" );
  close( fd );

  return EXIT_SUCCESS;
}
