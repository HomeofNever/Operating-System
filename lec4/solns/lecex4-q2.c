/* lecex4-q2.c */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#define MAXBUFFER 512

int main( int argc, char * argv[] )
{
  int port = 0;

  if ( argc == 2 )
  {
    port = atoi( argv[1] );
    if ( port < 1024 || port > 65535 ) port = 0;
  }

  int sd;  /* socket descriptor -- this is actually in the fd table! */

  /* create the socket (endpoint) on the server side */
  sd = socket( AF_INET, SOCK_DGRAM, 0 );
                    /*  ^^^^^^^^^^
                       this will set this socket up to use UDP */

  if ( sd == -1 )
  {
    perror( "socket() failed" );
    return EXIT_FAILURE;
  }

  printf( "Server-side socket created on descriptor %d\n", sd );


  struct sockaddr_in server;
  int length;

  server.sin_family = AF_INET;  /* IPv4 */

  server.sin_addr.s_addr = htonl( INADDR_ANY );
           /* any remote IP can send us a datagram */


  /* specify the port number for the server */
  server.sin_port = htons( port );

  /* bind to a specific (OS-assigned) port number */
  if ( bind( sd, (struct sockaddr *) &server, sizeof( server ) ) < 0 )
  {
    perror( "bind() failed" );
    return EXIT_FAILURE;
  }

  length = sizeof( server );

  /* call getsockname() to obtain the port number that was just assigned */
  if ( getsockname( sd, (struct sockaddr *) &server, (socklen_t *) &length ) < 0 )
  {
    perror( "getsockname() failed" );
    return EXIT_FAILURE;
  }

  printf( "UDP server at port number %d\n", ntohs( server.sin_port ) );






  /* the code below implements the application protocol */
  int n;
  char buffer[MAXBUFFER];
  struct sockaddr_in client;
  int len = sizeof( client );

  while ( 1 )
  {
    /* read a datagram from the remote client side (BLOCKING) */
    n = recvfrom( sd, buffer, MAXBUFFER - 1, 0, (struct sockaddr *) &client,
                  (socklen_t *) &len );

    if ( n == -1 )
    {
      perror( "recvfrom() failed" );
    }
    else
    {
      printf( "Rcvd datagram from %s port %d\n",
              inet_ntoa( client.sin_addr ), ntohs( client.sin_port ) );

      printf( "RCVD %d bytes\n", n );
      buffer[n] = '\0';   /* assume that its printable char[] data */
      printf( "RCVD: [%s]\n", buffer );

      int count = 0;
      for ( int i = 0 ; i < n ; i++ ) if ( buffer[i] == 'G' ) count++;
      int c = htonl( count );
      sendto( sd, &c, sizeof( int ), 0, (struct sockaddr *) &client, len );

      /* to do: check the return code of sendto() */
    }
  }

  return EXIT_SUCCESS;
}
