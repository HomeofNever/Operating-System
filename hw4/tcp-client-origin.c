/* tcp-client.c */

#include <sys/types.h>
#include <string.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <stdio.h>
#include <strings.h>
#include <unistd.h>

#define MAXBUFFER 8192

int main()
{
  /* create TCP client socket (endpoint) */
  int sd = socket( PF_INET, SOCK_STREAM, 0 );

  if ( sd == -1 )
  {
    perror( "socket() failed" );
    exit( EXIT_FAILURE );
  }

#if 0
  struct hostent * hp = gethostbyname( "localhost" );  /* 127.0.0.1 */
  struct hostent * hp = gethostbyname( "127.0.0.1" );
  struct hostent * hp = gethostbyname( "128.113.126.39" );
#endif

  struct hostent * hp = gethostbyname( "linux02.cs.rpi.edu" );

  if ( hp == NULL )
  {
    fprintf( stderr, "ERROR: gethostbyname() failed\n" );
    return EXIT_FAILURE;
  }

  struct sockaddr_in server;
  server.sin_family = AF_INET;
  memcpy( (void *)&server.sin_addr, (void *)hp->h_addr, hp->h_length );
  unsigned short port = 8123;
  server.sin_port = htons( port );

  printf( "CLIENT: server address is %s\n", inet_ntoa( server.sin_addr ) );


  printf( "CLIENT: connecting to server.....\n" );
  if ( connect( sd, (struct sockaddr *)&server, sizeof( server ) ) == -1 )
  {
    perror( "connect() failed" );
    return EXIT_FAILURE;
  }


  /* The implementation of the application protocol is below... */

  char * msg = "why not change the world?";
  printf( "CLIENT: Sending to server: [%s]\n", msg );
  int n = write( sd, msg, strlen( msg ) );    /* or send()/recv() */

  if ( n == -1 )
  {
    perror( "write() failed" );
    return EXIT_FAILURE;
  }

  char buffer[MAXBUFFER];
  n = read( sd, buffer, MAXBUFFER - 1 );    /* BLOCKING */

  if ( n == -1 )
  {
    perror( "read() failed" );
    return EXIT_FAILURE;
  }
  else if ( n == 0 )
  {
    printf( "CLIENT: Rcvd no data; also, server socket was closed\n" );
  }
  else  /* n > 0 */
  {
    buffer[n] = '\0';    /* assume we rcvd text-based data */
    printf( "CLIENT: Rcvd from server: [%s]\n", buffer );
  }

  printf( "CLIENT: Disconnecting...\n" );

  close( sd );

  return EXIT_SUCCESS;
}
