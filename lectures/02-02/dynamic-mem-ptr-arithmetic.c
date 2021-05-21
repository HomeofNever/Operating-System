/* dynamic-mem-ptr-arithmetic.c */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main()
{
  /* dynamically allocate 100 bytes (on the heap) */
  char * s = malloc( 100 );
     /* do not use malloc() in this course because */
     /*  it leaves memory uninitialized...         */

  /* dynamically allocate 100 bytes (on the heap) */
  char * t = calloc( 100, sizeof( char ) );
     /* calloc() will initialize these 100 bytes */
     /*  to be all zero bytes '\0'               */

  printf( "\"%s\" \"%s\"\n", s, t );

  free( s );
  free( t );


  int * v = calloc( 1, sizeof( int ) );
  free( v );


  /* dynamically allocate memory for an array
      of integers of size 47 */
#if 0
  int * int_array = malloc( 47 * sizeof( int ) );
#endif
  int * int_array = calloc( 47, sizeof( int ) );

  int_array[18] = 1234;

  printf( "%d\n", int_array[18] );
  printf( "%d\n", *(int_array + 18) );
                 /* int_array + 18*sizeof(int) */

#if 0
  printf( "%d\n", int_array[47000] );  /* causes a seg-fault... */
#endif

  free( int_array );


  /* array of strings (i.e., array of char arrays) */

  char ** names;   /* ... array of char* ... */
                   /* char * names[] */

  /*      malloc( 47 * sizeof( char * ) );   <== uninitialized */
  names = calloc( 47, sizeof( char * ) );
      /* TO DO: how many bytes are allocated? */

  names[2] = calloc( 7, sizeof( char ) );
  strcpy( names[2], "freyja" );
  printf( "Hi, %s\n", names[2] );

  /* use realloc() to expand names[2] */
  names[2] = realloc( names[2], 20 * sizeof( char ) );
  strcat( names[2], " and me" );
  printf( "Hi, %s\n", names[2] );


#if 0
  names[2] = NULL; /* this causes a memory leak */

                 char*
               +------+
  names---> [0]| NULL |
               +------+
            [1]| NULL |
               +------+   +----------+
            [2]| ========>|"freyja\0"|
               +------+   +----------+
                 ....
               +------+
           [46]| NULL |
               +------+

  if we include names[2]=NULL
   (or change names[2] to point to something else)...

                 char*
               +------+
  names---> [0]| NULL |
               +------+
            [1]| NULL |
               +------+   +----------+
            [2]| NULL |   |"freyja\0"|
               +------+   +----------+
                 ....
               +------+
           [46]| NULL |
               +------+
#endif

  free( names[0] );  /* no-op (see man page) */
  free( names[2] );
  free( names );

  return EXIT_SUCCESS;
}
