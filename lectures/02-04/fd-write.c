/* fd-write.c */

/* TO DO: write a corresponding fd-open.c example that reads in */
/*         the outfile.txt file and re-populates a variable of  */
/*          type student_t (also the int and short variables)   */

/* bash$ hexdump -C outfile.txt */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

int main()
{
  char * name = "outfile.txt";

                   /* attempt to open this file:
                       O_WRONLY for writing
                       O_CREAT create the file if necessary
                       O_TRUNC truncate the file if it already exists
                              (i.e., erase the contents, set size to 0)
                       (also O_APPEND) */
  int fd = open( name, O_WRONLY | O_CREAT | O_TRUNC, 0660 );
             /*                                      ^^^^
                                                      |
                                      leading 0 means this is in octal
                                                               (base 8)
                0660 ==> 110 110 000
                         rwx rwx rwx
                         ^^^ ^^^ ^^^
                          |   |   |
                          |   |  no permissions for other/world
                          |   |
                          | rw for group permissions
                          |
                     rw for file owner */


  printf( "fd is %d\n", fd );

  if ( fd == -1 )
  {
    perror( "open() failed" );
    return EXIT_FAILURE;
  }

  /* fd table:
       0 stdin
       1 stdout
       2 stderr
       3 outfile.txt (O_WRONLY)
   */

  char buffer[20];
  sprintf( buffer, "ABCD%dEFGH", fd );
  int rc = write( fd, buffer, strlen( buffer ) );
  printf( "wrote %d bytes to fd %d\n", rc, fd );

#if 0
  /* the write() system call does not buffer the output */
  int * x = NULL;
  *x = 1234;
#endif

/* TO DO: check out this URL: http://www.asciitable.com/ */

  rc = write( fd, buffer, 3 );
  printf( "wrote %d bytes to fd %d\n", rc, fd );

  /* write some binary data to the file... */
  int important = 32768;
  rc = write( fd, &important, sizeof( int ) );
  printf( "wrote %d bytes to fd %d\n", rc, fd );

  short q = 0xfade;  /* in memory (little endian): de fa */
  rc = write( fd, &q, sizeof( short ) );
  printf( "wrote %d bytes to fd %d\n", rc, fd );


  /* defining a data type (usually at the top of the source file) */
  typedef struct {
    long rin;
    char name[12];  /* <== not "best practice" */
  } student_t;

  student_t s1;  /* what (and how much) memory is allocated? */
  s1.rin = 660000007;
  strcpy( s1.name, "DAVID G" );

  printf( "sizeof( student_t ) is %lu\n", sizeof( student_t ) );

  rc = write( fd, &s1, sizeof( student_t ) );
  printf( "wrote %d bytes to fd %d\n", rc, fd );



  rc = write( fd, "last words\n", 11 );
  printf( "wrote %d bytes to fd %d\n", rc, fd );

  close( fd );

  return EXIT_SUCCESS;
}