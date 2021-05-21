/* pipe-with-fork.c */

/* A "pipe" is a unidirectional communication channel -- man 2 pipe */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

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
   */

  if ( rc == 0 )
  {
    /* write data to the pipe */
    int bytes_written = write( p[1], "ABCDEFGHIJKLMNOPQRSTUVWXYZ", 26 );
/******* context switch from the child process to the parent process *******/
    printf( "CHILD: Wrote %d bytes to the pipe\n", bytes_written );
  }
  else /* rc > 0 */
  {
    /* read data from the pipe */
    char buffer[80];
    int bytes_read = read( p[0], buffer, 10 );  /* BLOCKING CALL */
    buffer[bytes_read] = '\0';   /* assume the data is string data */
    printf( "PARENT: Read %d bytes: \"%s\"\n", bytes_read, buffer );
  }

  return EXIT_SUCCESS;
}

/* INITIAL POSSIBLE OUTPUTS:

goldsd@linux:~/s21$ ./a.out
Created pipe; p[0] is 3 and p[1] is 4
CHILD: Wrote 26 bytes to the pipe
PARENT: Read 10 bytes: "ABCDEFGHIJ"

goldsd@linux:~/s21$ ./a.out
Created pipe; p[0] is 3 and p[1] is 4
PARENT: Read 10 bytes: "ABCDEFGHIJ"
CHILD: Wrote 26 bytes to the pipe

 */
