/* two-words-v2.c */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

int main()
{
  close( 0 );   /* close stdin... */

  /* file descriptor table:
     0 -------
     1 stdout
     2 stderr
   */

  int fd = open( "infile.txt", O_RDONLY );

  if ( fd == -1 )
  {
    perror( "open() failed" );
    return EXIT_FAILURE;
  }

  /* file descriptor table:
     0 infile.txt (O_RDONLY)
     1 stdout
     2 stderr
   */


  char w1[20], w2[20];

  if ( isatty( STDIN_FILENO ) ) printf( "Enter two words: " );

  scanf( "%s %s", w1, w2 );

  close( fd );   /* close the "infile.txt" file */

  fprintf( stderr, "ERROR: this is not really an error.\n" );

  /*     +----------+        +----------+  */
  /* w1: | "abcd\0" |    w2: | "efgh\0" |  */
  /*     +----------+        +----------+  */

  printf( "w1 is %s and w2 is %s\n", w1, w2 );

  if ( strcmp( w1, w2 ) == 0 ) printf( "the words are the same\n" );
  else printf( "the words are different\n" );

  fprintf( stderr, "ERROR: this is also not an error.\n" );

  return EXIT_SUCCESS;
}
