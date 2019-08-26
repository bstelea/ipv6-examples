// IPv6 client server example_v2
// Client code

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <errno.h>

#include "provenance.h"

void error(char *msg) {
    perror(msg);
    exit(0);
}

int main(int argc, char* argv[]) {
  int sockfd, rv, n;
  struct addrinfo hints, *servinfo, *p;
  char buffer[256] = "This is a string from client!";

  if (provenance_set_tracked(true)) {
    printf("Failed Tracking, error %d\n", errno);
    printf("%s\n\n",strerror(errno));
  }
  if (provenance_set_propagate(true)) {
    printf("Failed propagate, error %d\n", errno);
    printf("%s\n\n",strerror(errno));
  }

  if (argc < 3) {
      fprintf(stderr, "Usage: <hostname> <port>%s  \n", argv[0]);
      exit(0);
  }

  printf("\nIPv6 TCP Client_v2 Started...\n");

  memset(&hints, 0, sizeof hints);
  hints.ai_family = AF_INET6;
  hints.ai_socktype = SOCK_STREAM;

  if ((rv = getaddrinfo(argv[1], argv[2], &hints, &servinfo)) != 0) {
      fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
      exit(1);
  }

  // loop through all the results and connect to the first we can
  for(p = servinfo; p != NULL; p = p->ai_next) {
      if ((sockfd = socket(p->ai_family, p->ai_socktype,
              p->ai_protocol)) == -1) {
          perror("socket");
          continue;
      }

      if (connect(sockfd, p->ai_addr, p->ai_addrlen) < 0) {
          perror("connect");
          close(sockfd);
          continue;
      }

      //Sockets Layer Call: send()
      n = send(sockfd,buffer, strlen(buffer)+1, 0);
      if (n < 0)
          error("ERROR writing to socket");

      memset(buffer, 0, 256);

      //Sockets Layer Call: recv()
      n = recv(sockfd, buffer, 255, 0);
      if (n < 0)
          error("ERROR reading from socket");
      printf("Message from server: %s\n", buffer);

      //Sockets Layer Call: close()
      close(sockfd);

      break; // if we get here, we must have connected successfully
  }

  if (p == NULL) {
      // looped off the end of the list with no connection
      fprintf(stderr, "failed to connect\n");
      exit(2);
  } 

  freeaddrinfo(servinfo); // all done with this structure
  return 0;
}
