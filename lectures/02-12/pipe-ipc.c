/* pipe-ipc.c */

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
    close( p[0] );  /* close the read end of the pipe (in the child) */

    /* write data to the pipe */
    int bytes_written = write( p[1], "ABCDEFGHIJKLMNOPQRSTUVWXYZ", 26 );
/******* context switch from the child process to the parent process *******/
    printf( "CHILD: Wrote %d bytes to the pipe\n", bytes_written );

    close( p[1] );

#if 0
    sleep( 10 );

    /* read data from the pipe */
    char buffer[80];
    int bytes_read = read( p[0], buffer, 10 );  /* BLOCKING CALL */
    buffer[bytes_read] = '\0';   /* assume the data is string data */
    printf( "CHILD: Read %d bytes: \"%s\"\n", bytes_read, buffer );
#endif
  }
  else /* rc > 0 */
  {
    close( p[1] );  /* close the write end of the pipe (in the parent) */

    /* read data from the pipe */
    char buffer[80];
    int bytes_read = read( p[0], buffer, 10 );  /* BLOCKING CALL */
    buffer[bytes_read] = '\0';   /* assume the data is string data */
    printf( "PARENT: Read %d bytes: \"%s\"\n", bytes_read, buffer );

    bytes_read = read( p[0], buffer, 10 );  /* BLOCKING CALL */
    buffer[bytes_read] = '\0';   /* assume the data is string data */
    printf( "PARENT: Read %d bytes: \"%s\"\n", bytes_read, buffer );

    bytes_read = read( p[0], buffer, 10 );  /* BLOCKING CALL */
    buffer[bytes_read] = '\0';   /* assume the data is string data */
    printf( "PARENT: Read %d bytes: \"%s\"\n", bytes_read, buffer );

    close( p[0] );

#if 0
    /* this fourth read() call will not block because:
       (a) there's no more data on the pipe to read
       (b) there are no active/open write descriptors on the pipe */
    bytes_read = read( p[0], buffer, 10 );
    buffer[bytes_read] = '\0';   /* assume the data is string data */
    printf( "PARENT: Read %d bytes: \"%s\"\n", bytes_read, buffer );
#endif
  }

  /* fd table (after closing the unused pipe descriptors):

     [PARENT]                                         [CHILD]
     0: stdin                                         0: stdin
     1: stdout                                        1: stdout
     2: stderr                +--------+              2: stderr
     3: p[0] <========READ====| buffer |              3:
     4:                       | buffer |<===WRITE==== 4: p[1]
                              +--------+
   */
  return EXIT_SUCCESS;
}

/* POSSIBLE OUTPUTS:

Created pipe; p[0] is 3 and p[1] is 4
CHILD: Wrote 26 bytes to the pipe
PARENT: Read 10 bytes: "ABCDEFGHIJ"
PARENT: Read 10 bytes: "KLMNOPQRST"
PARENT: Read 6 bytes: "UVWXYZ"

Created pipe; p[0] is 3 and p[1] is 4
PARENT: Read 10 bytes: "ABCDEFGHIJ"
CHILD: Wrote 26 bytes to the pipe
PARENT: Read 10 bytes: "KLMNOPQRST"
PARENT: Read 6 bytes: "UVWXYZ"

goldsd@linux:~/s21$ ./a.out
Created pipe; p[0] is 3 and p[1] is 4
PARENT: Read 10 bytes: "ABCDEFGHIJ"
PARENT: Read 10 bytes: "KLMNOPQRST"
CHILD: Wrote 26 bytes to the pipe
PARENT: Read 6 bytes: "UVWXYZ"

goldsd@linux:~/s21$ ./a.out
Created pipe; p[0] is 3 and p[1] is 4
PARENT: Read 10 bytes: "ABCDEFGHIJ"
PARENT: Read 10 bytes: "KLMNOPQRST"
PARENT: Read 6 bytes: "UVWXYZ"
CHILD: Wrote 26 bytes to the pipe




A -- PARENT: Read 10 bytes: "ABCDEFGHIJ"
B -- PARENT: Read 10 bytes: "KLMNOPQRST"
C -- PARENT: Read 6 bytes: "UVWXYZ"
D -- CHILD: Wrote 26 bytes to the pipe


                   Created pipe; p[0] is 3 and p[1] is 4
                                    |
                                    |
                                  fork()
                                 /      \
                                /        \
                               /          \
                       <PARENT>            <CHILD>
                       A                   D
                       B
                       C

 */









