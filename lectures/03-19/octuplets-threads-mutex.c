/* octuplets-threads-mutex.c */

#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>
#include <errno.h>
#include <time.h>
#include <pthread.h>

#define CHILDREN 8

/* global mutex variable */
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;


/* this function is called by each thread */
void * whattodo( void * arg );

int main()
{
  pthread_t tid[CHILDREN];    /* keep track of thread IDs */

  int i, rc, children = CHILDREN;

  for ( i = 0 ; i < children ; i++ )
  {
    int * t = malloc( sizeof( int ) );  /* TO DO: check that this succeeds... */

    *t = 2 + i * 2;   /* 2, 4, 6, 8, ... */

    printf( "MAIN: Next child thread will nap for %d seconds\n", *t );

    rc = pthread_create( &tid[i], NULL, whattodo, t );

    if ( rc != 0 )
    {
      fprintf( stderr, "pthread_create() failed (%d)\n", rc );
    }
  }


#if 1
  /* wait for child threads to terminate */
  for ( i = 0 ; i < CHILDREN ; i++ )
  {
    int * x;
    pthread_join( tid[i], (void **)&x );   /* BLOCK */
    printf( "MAIN: Joined a child thread that returned %d\n", *x );
    free( x );
  }
#endif

#if 0
  /* wait for child threads to terminate */
  for ( i = CHILDREN - 1 ; i >= 0 ; i-- )
  {
    int * x;
    pthread_join( tid[i], (void **)&x );   /* BLOCK */
    printf( "MAIN: Joined a child thread that returned %d\n", *x );
    free( x );
  }
#endif

  printf( "MAIN: All threads successfully joined.\n" );


  return EXIT_SUCCESS;
}


/* pretend that this function must be executed by no more than
 *  one thread at a time, i.e., we must have mutually exclusive
 *   access to this function
 */
void critical_section( int t )
{
  printf( "THREAD %lu: I'm gonna nap for %d seconds\n", pthread_self(), t );
  sleep( t );
  printf( "THREAD %lu: I'm awake!\n", pthread_self() );
}


/* each child process will sleep for t second,    */
/*  then return t as exit status when it wakes up */
void * whattodo( void * arg )
{
  int t = *(int *)arg;
  free( arg );

  pthread_mutex_lock( &mutex );
    critical_section( t );
  pthread_mutex_unlock( &mutex );

  /* dynamically allocate memory to hold a return value */
  int * return_value = malloc( sizeof( int ) );
  *return_value = t;

  pthread_exit( return_value );

  /* TO DO: modify this code (and the pthread_join() code in main() above)
            to send back a structure with two integers */

  return NULL;
}

