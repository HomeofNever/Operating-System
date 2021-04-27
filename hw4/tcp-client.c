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

int error() {
    fprintf(stderr, "ERROR: Invalid argument(s)\nUSAGE: a.out <server-hostname> <server-port> <n> <int-value-1> ... <int-value-n>")
    return EXIT_FAILURE
}

int main( int argc, char * argv[] )
{
  if (argc < 4) {
      return error();
  }

  char * hostname = argv[1];
  int inport = atoi(argv[2]);
  
  if (inport <= 0 || port > 65535) {
      return error(); 
  }

  int int_value_count = atoi(argv[3]);

  if (int_value_count <= 0) {
      return error();
  }

  if (argc - 4 != int_value_count) {
      return error();
  }

  int32_t int_values[int_value_count + 1];

  int_values[0] = htonl(int_value_count);
  for (int i = 1; i < int_value_count + 1; i++) {
    int x = atoi(argv[3 + i]);
    int_values[i] = htonl(x);
  }

  /* create TCP client socket (endpoint) */
  int sd = socket( PF_INET, SOCK_STREAM, 0 );

  if ( sd == -1 )
  {
    perror( "socket() failed" );
    exit( EXIT_FAILURE );
  }

  struct hostent * hp = gethostbyname( hostname );

  if ( hp == NULL )
  {
    fprintf( stderr, "ERROR: gethostbyname() failed\n" );
    return EXIT_FAILURE;
  }

  struct sockaddr_in server;
  server.sin_family = AF_INET;
  memcpy( (void *)&server.sin_addr, (void *)hp->h_addr, hp->h_length );
  unsigned short port = inport;
  server.sin_port = htons( port );

  if ( connect( sd, (struct sockaddr *)&server, sizeof( server ) ) == -1 )
  {
    perror( "connect() failed" );
    return EXIT_FAILURE;
  }

  printf("CLIENT: Connected to %s (port %d)\n", hostname, inport);

  /* The implementation of the application protocol is below... */
  printf("CLIENT: Sending %d integer value%s\n", int_value_count, int_value_count == 1 ? "" : "s");
  int n = write( sd, &int_values, (int_value_count + 1) * sizeof(int_values) );    /* or send()/recv() */

  if ( n == -1 )
  {
    perror( "write() failed" );
    return EXIT_FAILURE;
  }

  // First response
  int res = 0;
  n = read( sd, &res, sizeof(res));

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
    res = ntohl(res);
    printf( "SERVER: Rcvd response %d\n", res );
  }

  // Second response
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
    printf( "SERVER: Rcvd response \"%s\"\n", buffer );
  }

  printf("CLIENT: Disconnected from to %s (port %d)", hostname, inport);

  close( sd );

  return EXIT_SUCCESS;
}
