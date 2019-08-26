// IPv6 client server example_v2
// Server code

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <errno.h>

#include <arpa/inet.h>

#include "provenance.h"

void error(char *msg) {
    perror(msg);
    exit(1);
}

int main(int argc, char* argv[]) {
  int sockfd, newsockfd, rv, n, clilen;
  struct addrinfo hints, *servinfo, *p;
  char buffer[256];
  char client_addr_ipv6[100];

  if (provenance_set_tracked(true)) {
    printf("Failed Tracking, error %d\n", errno);
    printf("%s\n\n",strerror(errno));
  }
  if (provenance_set_propagate(true)) {
    printf("Failed propagate, error %d\n", errno);
    printf("%s\n\n",strerror(errno));
  }

  if (argc < 2) {
      fprintf(stderr, "Usage: %s <port>\n", argv[0]);
      exit(0);
  }

  printf("\nIPv6 TCP Server_v2 Started...\n");

  memset(&hints, 0, sizeof hints);
  hints.ai_family = AF_INET6;
  hints.ai_socktype = SOCK_STREAM;
  hints.ai_flags = AI_PASSIVE;

  if ((rv = getaddrinfo(NULL, argv[1], &hints, &servinfo)) != 0) {
      fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
      exit(1);
  }

  // loop through all the results and bind to the first we can
  for(p = servinfo; p != NULL; p = p->ai_next) {
      if ((sockfd = socket(p->ai_family, p->ai_socktype,
              p->ai_protocol)) == -1) {
          perror("socket");
          continue;
      }

      if (bind(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
          close(sockfd);
          perror("bind");
          continue;
      }

      break; // if we get here, we must have connected successfully
  }

  if (p == NULL) {
      // looped off the end of the list with no successful bind
      fprintf(stderr, "failed to bind socket\n");
      exit(2);
  } else {
    listen(sockfd,5);

    //Sockets Layer Call: accept()
    newsockfd = accept(sockfd, p->ai_addr, &(p->ai_addrlen));
    if (newsockfd < 0)
        error("ERROR on accept");

    struct sockaddr_in6* ipv6 = (struct sockaddr_in6 *) p->ai_addr;
    //Sockets Layer Call: inet_ntop()
    inet_ntop(AF_INET6, &(ipv6->sin6_addr), client_addr_ipv6, 100);
    printf("Incoming connection from client having IPv6 address: %s\n",client_addr_ipv6);

    memset(buffer,0, 256);

    //Sockets Layer Call: recv()
    n = recv(newsockfd, buffer, 255, 0);
    if (n < 0)
        error("ERROR reading from socket");

    printf("Message from client: %s\n", buffer);

    //Sockets Layer Call: send()
    n = send(newsockfd, "Server got your message", 23+1, 0);
    if (n < 0)
        error("ERROR writing to socket");

    //Sockets Layer Call: close()
    close(sockfd);
    close(newsockfd);
  }

  freeaddrinfo(servinfo); // all done with this structure

  return 0;
}
