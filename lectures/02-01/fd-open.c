/* fd-open.c */

/**
 *  file descriptor (fd)
 *
 *  -- each process has a file descriptor table
 *      associated with it that keeps track of its
 *       open files (i.e., byte streams)
 *
 *  fd        C++   Java        C
 *  0 stdin   cin   System.in   scanf(), read(), getchar(), ...
 *  1 stdout  cout  System.out  printf(), write(), putchar(), ...
 *  2 stderr  cerr  System.err  fprintf( stderr, "ERROR: ....\n" );
 *                              perror( "open() failed" );
 *
 *     stdout and stderr (by default) are both displayed on the terminal
 *
 *     stdout is line-buffered when output to the terminal
 *
 *     stdout is block-buffered when output is redirected to a file
 *
 *     stderr (fd 2) is unbuffered
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

int main()
{
  char * name = "testfile.txt";

  int fd = open( name, O_RDONLY );

  /* TO DO: create an example using O_WRONLY that writes    */
  /*         data to a file (and try using write() and also */
  /*          printf()/fprintf() to write to the file...)   */

  printf( "fd is %d\n", fd );

  if ( fd == -1 )
  {
    perror( "open() failed" );
    return EXIT_FAILURE;        /* exit( EXIT_FAILURE ); */
  }

  /* fd table for this running process:
       0 stdin
       1 stdout
       2 stderr
       3 testfile.txt (O_RDONLY)
   */

  char buffer[20];
  int rc = read( fd, buffer, 19 );
  buffer[rc] = '\0';   /* assume the data is character data */
  printf( "read returned %d -- read [%s]\n", rc, buffer );

  rc = read( fd, buffer, 19 );
  buffer[rc] = '\0';   /* assume the data is character data */
  printf( "read returned %d -- read [%s]\n", rc, buffer );

  rc = read( fd, buffer, 19 );
  buffer[rc] = '\0';   /* assume the data is character data */
  printf( "read returned %d -- read [%s]\n", rc, buffer );

  /* at some point, we will reach the end of the file and       */
  /*  read() will return 0 --- check out the man page for read! */

  rc = read( fd, buffer, 19 );
  buffer[rc] = '\0';   /* assume the data is character data */
  printf( "read returned %d -- read [%s]\n", rc, buffer );

  close( fd );

  /* fd table for this running process:
       0 stdin
       1 stdout
       2 stderr
       3 
   */

  rc = read( fd, buffer, 19 );
  buffer[rc] = '\0';   /* assume the data is character data */
  printf( "read returned %d -- read [%s]\n", rc, buffer );

  if ( rc == -1 )
  {
    perror( "read() failed" );
  }

  return EXIT_SUCCESS;
}
