/* endian.c */

/* to look at the individual bytes of the output file:

   bash$ hexdump endian.dat
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <arpa/inet.h>

int main()
{
  short x = 720;  /* 2-byte representation */
    /*      binary: 0000 0010 1101 0000 */
    /* hexadecimal:    0    2    d    0 */

    /* little endian bytes: d0 02 */
    /*    big endian bytes: 02 d0 */

  int fd = open( "endian.dat", O_WRONLY | O_CREAT | O_TRUNC, 0660 );
  if ( fd == -1 ) { perror( "open() failed" ); return EXIT_FAILURE; }

  int rc = write( fd, &x, sizeof( short ) );
  if ( rc == -1 ) { perror( "write() failed" ); return EXIT_FAILURE; }

  short z = htons( x );

  rc = write( fd, &z, sizeof( short ) );
  if ( rc == -1 ) { perror( "write() failed" ); return EXIT_FAILURE; }

  close( fd );

  /* TO DO: repeat this but use int variables and htonl() function */

  return EXIT_SUCCESS;
}
