/* prep-Q15.c */

/* see corresponding video, to be posted soon...! */

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
int parent( int * p );

int main()
{
  int p[4];   /* array to hold the four pipe (file) descriptors */

  int pipe_rc = pipe( p );  /* &p[0] */
  if ( pipe_rc == -1 ) { perror( "pipe() failed" ); return EXIT_FAILURE; }

  pipe_rc = pipe( &p[2] );  /* p + 2 */
  if ( pipe_rc == -1 ) { perror( "pipe() failed" ); return EXIT_FAILURE; }

  /* fd table:

     0: stdin
     1: stdout
     2: stderr                +--------+
     3: p[0] <========READ====| buffer | think of this buffer as a
     4: p[1] =========WRITE==>| buffer |  temporary hidden transient file
                              +--------+
     5: p[2] <========READ====| buffer | think of this buffer as a
     6: p[3] =========WRITE==>| buffer |  temporary hidden transient file
                              +--------+
   */


  pid_t rc = fork();  /* fork() will copy the fd table to the child */
  if ( rc == -1 ) { perror( "fork() failed" ); return EXIT_FAILURE; }

  /* fd table:

     [PARENT]                                         [CHILD]
     0: stdin                                         0: stdin
     1: stdout                                        1: stdout
     2: stderr                +--------+              2: stderr
     3: p[0] <========READ====| buffer |====READ====> 3: p[0]
     4: p[1] =========WRITE==>| buffer |<===WRITE==== 4: p[1]
                              +--------+
     5: p[2] <========READ====| buffer |====READ====> 5: p[2]
     6: p[3] =========WRITE==>| buffer |<===WRITE==== 6: p[3]
                              +--------+

   */

  if ( rc == 0 )
  {
    rc = child( p );
  }
  else /* rc > 0 */
  {
    rc = parent( p );
  }

  return rc;
}

int child( int * p )
{
  close( p[1] );  /* close the write end of the first pipe */
  close( p[2] );  /* close the read end of the second pipe */

  int rc = EXIT_SUCCESS;

  while ( 1 )
  {
    char buffer[MAXBUFFER];
    int bytes_read = read( p[0], buffer, MAXBUFFER - 1 );  /* BLOCKING CALL */

    if ( bytes_read == -1 ) { perror( "CHILD: read() failed on pipe" ); rc = EXIT_FAILURE; break; }
    else if ( bytes_read == 0 ) { rc = EXIT_SUCCESS; break; }
    else
    {
      buffer[bytes_read] = '\0';

      char result[MAXBUFFER];
      result[0] = '\0';
      for ( char * ptr = buffer ; *ptr ; ptr++ ) if ( isalpha( *ptr ) || *ptr == '\n' ) strncat( result, ptr, 1 );

      rc = write( p[3], result, strlen( result ) );
      if ( rc == -1 ) { perror( "CHILD: write() failed on pipe" ); rc = EXIT_FAILURE; break; }
    }
  }

  close( p[0] );
  close( p[3] );

  return rc;
}

int parent( int * p )
{
  close( p[0] );  /* close the read end of the first pipe */
  close( p[3] );  /* close the write end of the second pipe */

  int fd = open( "alpha.txt", O_WRONLY | O_CREAT | O_TRUNC, 0600 );
  if ( fd == -1 ) { perror( "PARENT: open() failed" ); return EXIT_FAILURE; }

  int rc = EXIT_SUCCESS;

  while ( 1 )
  {
    char buffer[MAXBUFFER];

    if ( fgets( buffer, MAXBUFFER, stdin ) == NULL ) { rc = EXIT_SUCCESS; break; }

    write( p[1], buffer, strlen( buffer ) );

    int bytes_read = read( p[2], buffer, MAXBUFFER - 1 );  /* BLOCKING CALL */

    if ( bytes_read == -1 ) { perror( "PARENT: read() failed on the pipe" ); rc = EXIT_FAILURE; break; }
    else if ( bytes_read == 0 ) { rc = EXIT_FAILURE; break; }
    else
    {
      buffer[bytes_read] = '\0';   /* assume the data is string data */
      rc = write( fd, buffer, bytes_read );
      if ( rc == -1 ) { perror( "PARENT: write() failed on the file" ); rc = EXIT_FAILURE; break; }
    }
  }

  close( fd );
  close( p[1] );
  close( p[2] );
  waitpid( -1, NULL, 0 );  /* after close() of pipes! */

  return rc;
}
