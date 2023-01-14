#include <stdio.h>
#include <strings.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

int main(void)
{
  int tcpSd;
  struct sockaddr_in s_addr;

  if ((tcpSd = socket(PF_INET, SOCK_STREAM, 0)) < 0) {
    perror("socket()");
    exit(-1);
  }

  bzero((char *)&s_addr, sizeof(s_addr));
  s_addr.sin_family = AF_INET;
  inet_aton("203.249.39.3", &s_addr.sin_addr);
  s_addr.sin_port = htons(7);

  printf("ip(dotted decmial) = %s\n", inet_ntoa(s_addr.sin_addr));
  printf("ip(binary) = 0x%x\n", ntohl(s_addr.sin_addr.s_addr));
  printf("port no = %d\n", ntohs(s_addr.sin_port));

  return 0;
}