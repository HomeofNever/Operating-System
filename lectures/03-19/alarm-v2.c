/* alarm-v2.c */

/* implementation using multiple processes, i.e., each child
 *  process will essentially "run" one alarm
 */


#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

int main()
{
  const int maxline = 80;
  char * line = calloc( maxline, sizeof( char ) );
  char * msg = calloc( maxline, sizeof( char ) );

  char * format_string = calloc( maxline, sizeof( char ) );
  sprintf( format_string, "%%d %%%d[^\n]", maxline - 1 );

#if 0
printf( "[%s]\n", format_string );
#endif

  while ( 1 )
  {
    int seconds;

    printf( "Set alarm (<sec> <msg>) -- 0 to exit: " );

    if ( fgets( line, maxline - 1, stdin ) == NULL ) return EXIT_FAILURE;

    if ( strlen( line ) <= 1 ) continue;   /* skip blank lines */

    if ( sscanf( line, format_string, &seconds, msg ) < 1 || seconds < 0 )
    {
      fprintf( stderr, "ERROR: invalid alarm request\n" );
    }
    else if ( seconds == 0 )
    {
      break;
    }
    else
    {
      /* create a child process to create (and wait) for the alarm */
      pid_t pid = fork();

      if ( pid == -1 )
      {
        perror( "fork() failed" );
        return EXIT_FAILURE;
      }
      else if ( pid == 0 )   /* CHILD */
      {
        free( line );
        free( format_string );
        printf( "Alarm set for %d seconds\n", seconds );
        sleep( seconds );
        printf( "ALARM (%d): %s\n", seconds, msg );
        msg[0] = '\0';
        free( msg );
        exit( EXIT_SUCCESS );
      }
      else
      {
        /* TO DO: add waitpid() with the WNOHANG option to kill zombies */
        usleep( 10000 );
      }
    }
  }

  free( msg );
  free( line );
  free( format_string );

  return EXIT_SUCCESS;
}
