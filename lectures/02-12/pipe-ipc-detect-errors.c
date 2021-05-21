/* pipe-ipc-detect-errors.c */

/* A "pipe" is a unidirectional communication channel -- man 2 pipe */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

int main()
{
  int p[2];   /* array to hold the two pipe (file) descriptors:
                 p[0] is the "read" end of the pipe
                 p[1] is the "write" end of the pipe */

  int pipe_rc = pipe( p );  /* the input argument p will be filled in
                                with the assigned pipe/file descriptors */

  if ( pipe_rc == -1 )
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


  pid_t rc = fork();  /* fork() will copy the fd table to the child */

  if ( rc == -1 )
  {
    perror( "fork() failed" );
    return EXIT_FAILURE;
  }

  /* fd table:

     [PARENT]                                         [CHILD]
     0: stdin                                         0: stdin
     1: stdout                                        1: stdout
     2: stderr                +--------+              2: stderr
     3: p[0] <========READ====| buffer |====READ====> 3: p[0]
     4: p[1] =========WRITE==>| buffer |<===WRITE==== 4: p[1]
                              +--------+

     after the two calls to close() in parent and child below,
      the fd table looks like this:

     [PARENT]                                         [CHILD]
     0: stdin                                         0: stdin
     1: stdout                                        1: stdout
     2: stderr                +--------+              2: stderr
     3: p[0] <========READ====| buffer |              3:
     4:                       | buffer |<===WRITE==== 4: p[1]
                              +--------+

     after the child process is done writing and terminates
      --or-- the child process seg-faults/terminates:

     [PARENT]
     0: stdin
     1: stdout
     2: stderr                +--------+
     3: p[0] <========READ====| buffer |  (no write descriptors
     4:                       | buffer |    associated with this pipe)
                              +--------+

   */

  if ( rc == 0 )
  {
    close( p[0] );  /* close the read end of the pipe (in the child) */

    char * xyz = NULL;
    strcpy( xyz, "QRSTUVWXYZ" );  /* segmentation fault */

    /* write data to the pipe */
    int bytes_written = write( p[1], "ABCDEFGHIJKLMNOPQRSTUVWXYZ", 26 );
    printf( "CHILD: Wrote %d bytes to the pipe\n", bytes_written );

    close( p[1] );
  }
  else /* rc > 0 */
  {
    close( p[1] );  /* close the write end of the pipe (in the parent) */

    /* read data from the pipe */
    char buffer[80];
    int bytes_read = read( p[0], buffer, 10 );  /* BLOCKING CALL */

    if ( bytes_read == -1 )
    {
      perror( "read() failed on the pipe" );
      return EXIT_FAILURE;
    }
    else if ( bytes_read == 0 )
    {
      printf( "PARENT: there are no write descriptors on the pipe\n" );
      printf( "PARENT: child process must have terminated....\n" );
      return EXIT_FAILURE;
    }
    else
    {
      buffer[bytes_read] = '\0';   /* assume the data is string data */
      printf( "PARENT: Read %d bytes: \"%s\"\n", bytes_read, buffer );
      close( p[0] );
    }
  }

  return EXIT_SUCCESS;
}

