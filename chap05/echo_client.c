//echo_client.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#define PORT 9000
#define IPADDR "127.0.0.1"

int main(void)
{
  int c_socket;
  struct sockaddr_in c_addr;
  int len, n;
  int n_left, n_recv;

  char rcvBuffer[BUFSIZ];
  char sndBuffer[BUFSIZ];

  c_socket=socket(AF_INET, SOCK_STREAM, 0);

  memset(&c_addr, 0, sizeof(c_addr));
  c_addr.sin_family = AF_INET;
  c_addr.sin_addr.s_addr = inet_addr(IPADDR);
  c_addr.sin_port = htons(PORT);

  if (connect(c_socket, (struct sockaddr *)&c_addr, sizeof(c_addr)) < 0) {
      perror("Connect error");
      close(c_socket);
      exit(0);
  }

  while (1) {
    if ((n = read(0, sndBuffer, sizeof(sndBuffer))) > 0) {
      sndBuffer[n] = '\0';
      if (!strcmp(sndBuffer, "quit\n")) break;

      printf("Original Data: %s", sndBuffer);
      if ((n = write(c_socket, sndBuffer, strlen(sndBuffer))) < 0) {
        exit (-1);
      }

      n_left = n;
      n_recv = 0;
      while (n_left > 0) {
        if ((n = read(c_socket, &rcvBuffer[n_recv], n_left)) < 0) {
          exit (-1);
        }
        n_left -= n;
        n_recv += n;
      }

      rcvBuffer[n_recv] = '\0';
      printf("Echoed Data: %s", rcvBuffer);
    }
  }
  close(c_socket);

  return 0;

}