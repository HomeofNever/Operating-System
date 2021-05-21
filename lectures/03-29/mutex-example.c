/* mutex-example.c */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>

/* shared global variables */
int global_a;    /* sum: 1 + 2 + ... + a */
int global_b;    /* sum: 1 + 2 + ... + b */
int global_sum;  /* global_a + global_b */

/* function prototypes for the thread functions */
void * whattodo_for_a( void * arg );
void * whattodo_for_b( void * arg );

/* global mutex variables */
pthread_mutex_t mutex_on_global_sum = PTHREAD_MUTEX_INITIALIZER;

#if 0
pthread_mutex_t mutex_on_global_b = PTHREAD_MUTEX_INITIALIZER;
#endif

int main( int argc, char * argv[] )
{
  if ( argc != 3 )
  {
    fprintf( stderr, "ERROR: Invalid input(s)\n" );
    fprintf( stderr, "USAGE: %s <a> <b>\n", argv[0] );
    return EXIT_FAILURE;
  }

  int a = atoi( argv[1] );
  int b = atoi( argv[2] );

  int expected_sum_a = 0;
  for ( int i = 1 ; i <= a ; i++ ) expected_sum_a += i;

  int expected_sum_b = 0;
  for ( int i = 1 ; i <= b ; i++ ) expected_sum_b += i;

  int expected_global_sum = expected_sum_a + expected_sum_b;


  global_sum = global_a = global_b = 0;

  pthread_t tid1, tid2;
  int rc;

  rc = pthread_create( &tid1, NULL, whattodo_for_a, &a );
  if ( rc != 0 ) { fprintf( stderr, "pthread_create() failed (%d)\n", rc ); return EXIT_FAILURE; }

  rc = pthread_create( &tid2, NULL, whattodo_for_b, &b );
  if ( rc != 0 ) { fprintf( stderr, "pthread_create() failed (%d)\n", rc ); return EXIT_FAILURE; }

  /* if we add a third call to pthread_create() and have that
      child thread call whattodo_for_b(), then global_b will
       require synchronization */

  rc = pthread_join( tid1, NULL );
  if ( rc != 0 ) { fprintf( stderr, "pthread_join() failed (%d)\n", rc ); return EXIT_FAILURE; }

  rc = pthread_join( tid2, NULL );
  if ( rc != 0 ) { fprintf( stderr, "pthread_join() failed (%d)\n", rc ); return EXIT_FAILURE; }


  printf( "MAIN: global a is %d (expected %d)\n", global_a, expected_sum_a );
  printf( "MAIN: global b is %d (expected %d)\n", global_b, expected_sum_b );
  printf( "MAIN: global sum is %d (expected %d)\n", global_sum, expected_global_sum );

  return EXIT_SUCCESS;
}

void * whattodo_for_a( void * arg )
{
  int a = *(int *)arg;

  for ( int i = 1 ; i <= a ; i++ )
  {
    printf( "Adding %d to global_a\n", i );
    global_a += i;
      /* the shared global_a variable is only updated here
          in this function -- and this thread is the only
           thread to run this function */

    usleep( 4000 );
    printf( "Adding %d to global_sum\n", i );

    pthread_mutex_lock( &mutex_on_global_sum );
    {
      /******************CRITICAL SECTION************************/
      global_sum += i;
      /******************CRITICAL SECTION************************/
    }
    pthread_mutex_unlock( &mutex_on_global_sum );
  }

  return NULL;
}

void * whattodo_for_b( void * arg )
{
  int b = *(int *)arg;

  for ( int i = 1 ; i <= b ; i++ )
  {
    printf( "Adding %d to global_b\n", i );
    global_b += i;
      /* the shared global_b variable is only updated here
          in this function -- and this thread is the only
           thread to run this function */

    printf( "Adding %d to global_sum\n", i );

    pthread_mutex_lock( &mutex_on_global_sum );
    {
      /******************CRITICAL SECTION************************/
      /*    global_sum += i; */
      int tmp = global_sum;
      usleep( 4000 );
      tmp += i;
      global_sum = tmp;
      /******************CRITICAL SECTION************************/
    }
    pthread_mutex_unlock( &mutex_on_global_sum );
  }

  return NULL;
}
