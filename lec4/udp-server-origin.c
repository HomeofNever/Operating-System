/* udp-server.c */

/* Sockets-based communication:

   -- a SOCKET is an endpoint for communication
                     ^^^^^^^^

   -- sockets-based communication can be connection-oriented or connection-less

   -- a SERVER creates one (or more) sockets that it will then LISTEN on for
       incoming client connection requests (TCP) or incoming datagrams (UDP)

   -- the server listens on one (or more) specific PORT numbers (2-byte short)
*/


/* UDP SERVER example
   -- socket()             create a socket (endpoint) for communication
   -- bind()               bind to (assign) a specific port number
                           (or let the OS assign us a port number)
   -- getsockname()        get socket "name" -- IP address, port number, etc.
   -- recvfrom()/sendto()  receive/send datagrams
*/


/* To test this server, you can use the following
   command-line netcat tool:


   bash$ netcat -u linux02.cs.rpi.edu 41234
                   ^^^^^^
                   replace with your hostname

   Note that netcat will act as a client to this UDP server...

   The hostname (e.g., linux02.cs.rpi.edu) cannot be
   localhost (127.0.0.1); and the port number must match what
   the server reports.

*/


#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#define MAXBUFFER 8192

int main()
{
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
  server.sin_port = htons( 0 );  /* a 0 here means let the kernel assign
                                     us a port number to listen on */

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
  char buffer[ MAXBUFFER ];
  struct sockaddr_in client;
  int len = sizeof( client );

  while ( 1 )
  {
    /* read a datagram from the remote client side (BLOCKING) */
    n = recvfrom( sd, buffer, MAXBUFFER, 0, (struct sockaddr *) &client,
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

      /* echo the first 4 bytes (at most) plus '\n' back to the sender/client */
      if ( n > 4 ) { n = 5; buffer[4] = '\n'; }
      sendto( sd, buffer, n, 0, (struct sockaddr *) &client, len );

      /* to do: check the return code of sendto() */
    }
  }

  return EXIT_SUCCESS;
}
