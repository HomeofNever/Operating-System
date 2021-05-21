/* tcp-server-multiplex.c */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/select.h>

#define MAXBUFFER 8192
#define MAX_CLIENTS 128

int main()
{
  fd_set readfds;
  int client_sockets[MAX_CLIENTS]; /* client socket fd list */
  int client_socket_index = 0;  /* next free spot */


  /* Create the listener socket as TCP socket (SOCK_STREAM) */
  int listener = socket( PF_INET, SOCK_STREAM, 0 );
    /* here, the listener is a socket descriptor (part of the fd table) */

  if ( listener == -1 )
  {
    perror( "socket() failed" );
    return EXIT_FAILURE;
  }


  /* populate the socket structure for bind() */
  struct sockaddr_in server;
  server.sin_family = PF_INET;  /* AF_INET */

  server.sin_addr.s_addr = htonl( INADDR_ANY );
    /* allow any IP address to connect */

  unsigned short port = 8123;

  /* htons() is host-to-network short for data marshalling */
  /* Internet is big endian; Intel is little endian; etc.  */
  server.sin_port = htons( port );
  int len = sizeof( server );

  /* attempt to bind (or associate) port 8123 with the socket */
  if ( bind( listener, (struct sockaddr *)&server, len ) == -1 )
  {
    perror( "bind() failed" );
    return EXIT_FAILURE;
  }


  /* identify this port as a TCP listener port */
  if ( listen( listener, 5 ) == -1 )
  {
    perror( "listen() failed" );
    return EXIT_FAILURE;
  }

  printf( "SERVER: TCP listener socket (fd %d) bound to port %d\n", listener, port );



  int n;
  char buffer[MAXBUFFER];

  while ( 1 )
  {
#if 0
    struct timeval timeout;
    timeout.tv_sec = 2;
    timeout.tv_usec = 500;  /* 2 seconds AND 500 microseconds */
#endif

    FD_ZERO( &readfds );
    FD_SET( listener, &readfds );   /* listener socket, fd 3 */
    printf( "SERVER: Set FD_SET to include listener fd %d\n", listener );

    /* initially, this for loop does nothing; but once we have some */
    /*  client connections, we will add each client connection's fd */
    /*   to the readfds (the FD set) */
    for ( int i = 0 ; i < client_socket_index ; i++ )
    {
      FD_SET( client_sockets[i], &readfds );
      printf( "SERVER: Set FD_SET to include client socket fd %d\n", client_sockets[i] );
    }

    printf( "SERVER: Blocked on select()...\n" );
#if 1
    /* This is a BLOCKING call, but will block on all readfds */
    int ready = select( FD_SETSIZE, &readfds, NULL, NULL, NULL );
#endif

#if 0
    int ready = select( FD_SETSIZE, &readfds, NULL, NULL, &timeout );

    if ( ready == 0 )
    {
      printf( "No activity (yet)...\n" );
      continue;
    }
#endif

    /* ready is the number of ready file descriptors */
    printf( "SERVER: select() identified %d descriptor(s) with activity\n", ready );


    /* is there activity on the listener descriptor? */
    if ( FD_ISSET( listener, &readfds ) )
    {
      struct sockaddr_in client;
      int fromlen = sizeof( client );

      /* we know this accept() call will not block! */
      /* printf( "SERVER: Blocked on accept()\n" ); */
      int newsd = accept( listener, (struct sockaddr *)&client, (socklen_t *)&fromlen );

      if ( newsd == -1 )
      {
        perror( "accept() failed" );
        continue;
      }

      printf( "SERVER: Accepted client connection from %s\n", inet_ntoa( (struct in_addr)client.sin_addr ) );
      client_sockets[client_socket_index++] = newsd;
    }


    /* is there activity on any of the established connections? */
    for ( int i = 0 ; i < client_socket_index ; i++ )
    {
      int fd = client_sockets[i];

      if ( FD_ISSET( fd, &readfds ) )
      {
        /* we know this recv() call will not block! */
        /* printf( "SERVER: Blocked on recv()\n" ); */
        /* recv() call will block until we receive data (n > 0)
            or an error occurs (n == -1)
             or the client closed its socket (n == 0) */
        n = recv( fd, buffer, MAXBUFFER - 1, 0 );   /* or read() */

        if ( n == -1 )
        {
          perror( "recv() failed" );
          return EXIT_FAILURE;
        }
        else if ( n == 0 )
        {
          printf( "SERVER: Rcvd 0 from recv(); closing client socket...\n" );
          close( fd );

          /* remove fd from client_sockets[] array: */
          for ( int k = 0 ; k < client_socket_index ; k++ )
          {
            if ( fd == client_sockets[k] )
            {
              /* found it -- copy remaining elements over fd */
              for ( int m = k ; m < client_socket_index - 1 ; m++ )
              {
                client_sockets[m] = client_sockets[m + 1];
              }
              client_socket_index--;
              break;  /* all done */
            }
          }
        }
        else /* n > 0 */
        {
          buffer[n] = '\0';   /* assume this is text data */
          printf( "SERVER: Rcvd message from: [%s]\n", buffer );

          printf( "SERVER: Sending acknowledgement to client\n" );
          /* send OK message back to client */
          n = send( fd, "OK", 2, 0 );

          if ( n == -1 )
          {
            perror( "send() failed" );
            return EXIT_FAILURE;
          }
        }
      }
    }
  }

  return EXIT_SUCCESS;
}
