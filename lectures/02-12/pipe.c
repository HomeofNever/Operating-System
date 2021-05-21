/* pipe.c */

/* A "pipe" is a unidirectional communication channel -- man 2 pipe */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main()
{
  int p[2];   /* array to hold the two pipe (file) descriptors:
                 p[0] is the "read" end of the pipe
                 p[1] is the "write" end of the pipe */

  int rc = pipe( p );  /* the input argument p will be filled in
                           with the assigned pipe/file descriptors */

  if ( rc == -1 )
  {
    perror( "pipe() failed" );
    return EXIT_FAILURE;
  }

  /* fd table:

     0: stdin
     1: stdout
     2: stderr                +--------+
     3: p[0] <========READ====| buffer | think of this buffer as a
     4: p[1] =========WRITE==>| buffer |  temporary hidden transient file
                              +--------+
   */

  printf( "Created pipe; p[0] is %d and p[1] is %d\n", p[0], p[1] );

  /* write data to the pipe */
  int bytes_written = write( p[1], "ABCDEFGHIJKLMNOPQRSTUVWXYZ", 26 );
  printf( "Wrote %d bytes to the pipe\n", bytes_written );

  /* read data from the pipe */
  char buffer[80];
  int bytes_read = read( p[0], buffer, 10 );
  buffer[bytes_read] = '\0';   /* assume the data is string data */
  printf( "Read %d bytes: \"%s\"\n", bytes_read, buffer );

  bytes_read = read( p[0], buffer, 10 );
  buffer[bytes_read] = '\0';   /* assume the data is string data */
  printf( "Read %d bytes: \"%s\"\n", bytes_read, buffer );

  bytes_read = read( p[0], buffer, 10 );
  buffer[bytes_read] = '\0';   /* assume the data is string data */
  printf( "Read %d bytes: \"%s\"\n", bytes_read, buffer );


#if 0
  /* the read() call below will BLOCK because there's no data on the pipe
   *  and there is at least one active/open "write" fd on the pipe
   */
  bytes_read = read( p[0], buffer, 10 );
  buffer[bytes_read] = '\0';   /* assume the data is string data */
  printf( "Read %d bytes: \"%s\"\n", bytes_read, buffer );
#endif

  close( p[1] );   /* close the "write" end of the pipe */

  /* fd table:

     0: stdin
     1: stdout
     2: stderr                +--------+
     3: p[0] <========READ====| buffer | think of this buffer as a
     4:                       | buffer |  temporary hidden transient file
                              +--------+
   */

  /* the read() call below will not BLOCK because
   *  there are zero active/open "write" fds on the pipe
   */
  bytes_read = read( p[0], buffer, 10 );
  buffer[bytes_read] = '\0';   /* assume the data is string data */
  printf( "Read %d bytes: \"%s\"\n", bytes_read, buffer );

  close( p[0] );

  /* fd table:

     0: stdin
     1: stdout
     2: stderr
   */

  return EXIT_SUCCESS;
}




