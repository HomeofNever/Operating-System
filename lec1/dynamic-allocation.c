/* dynamic-allocation.c */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

int main()
{
  char * path = malloc( 20 );  /* dynamically allocate
                                   20 bytes on the heap
                                    (use sizeof here) */

  strcpy( path, "/csci/goldsd/s21/" );
  printf( "path is %s\n", path );


  char * path2 = malloc( 20 );
  strcpy( path2, "AAAAAAAAAAAAAA" );
  printf( "path2 is %s\n", path2 );

  strcpy( path, "/csci/goldsd/s21/blah/blah/blah/meme" );
  printf( "path is %s\n", path );
  printf( "path2 is %s\n", path2 );

  /* why do the two printf() lines above show strange output? */

  free( path );
  free( path2 );

  return EXIT_SUCCESS;
}