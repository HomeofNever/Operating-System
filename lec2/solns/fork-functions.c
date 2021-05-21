/* fork-functions.c */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/wait.h>

int lecex2_parent()
{
  /* wait for the child process to terminate */
  int status;
  waitpid( -1, &status, 0 );   /* BLOCKING CALL */

  /* to do: check the rc from wait() for error... */

  if ( WIFSIGNALED( status ) )
  {
    printf( "PARENT: child process terminated abnormally\n" );
    return EXIT_FAILURE;
  }
  else /* if ( WIFEXITED( status ) ) */
  {
    int exit_status = WEXITSTATUS( status );
    printf( "PARENT: child process reported '%c'\n", exit_status );
  }

  return EXIT_SUCCESS;
}

int lecex2_child()
{
  /* Attempt to open a file called "data.txt" */
  int fd = open( "data.txt", O_RDONLY );

  if ( fd == -1 )
  {
    perror( "open() failed" );
    abort();
  }

  /* Attempt to read the sixth character from the file */
  int rc = lseek( fd, 5, SEEK_SET );

  if ( rc == -1 )
  {
    perror( "lseek() failed" );
    abort();
  }

  char c;
  rc = read( fd, &c, 1 );

  if ( rc == -1 )
  {
    perror( "read() failed" );
    abort();
  }
  else if ( rc == 0 )
  {
    fprintf( stderr, "file too small\n" );
    abort();
  }

  /* exit the child process and return the sixth character as
     its exit status to the parent */

  exit( c );
}
