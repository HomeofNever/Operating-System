/* two-words-v1.c */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

int main()
{
  char w1[20], w2[20];

  if ( isatty( STDIN_FILENO ) ) printf( "Enter two words: " );

  scanf( "%s %s", w1, w2 );

  /*     +----------+        +----------+  */
  /* w1: | "abcd\0" |    w2: | "efgh\0" |  */
  /*     +----------+        +----------+  */

  printf( "w1 is %s and w2 is %s\n", w1, w2 );

  if ( strcmp( w1, w2 ) == 0 ) printf( "the words are the same\n" );
  else printf( "the words are different\n" );

  return EXIT_SUCCESS;
}
