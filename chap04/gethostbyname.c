#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <netdb.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

int main(void)
{
  struct in_addr addr;
  struct hostent *host;
  const char *hostName = "www.naver.com";
  int i;

  if ((host = gethostbyname(hostName)) == NULL) {
    printf("gethostbyname() error - check network\n");
    exit(-1);
  }

  printf("official name = %s\n", host->h_name);

  i = 0;
  while (host->h_aliases[i] != NULL) {
    printf("aliases = %s\n", host->h_aliases[i++]);
  }

  printf("address type = %d\n", host->h_addrtype);
  printf("address length = %d\n", host->h_length);

  i = 0;
  while (host->h_addr_list[i] != NULL) {
    memcpy(&addr.s_addr, host->h_addr_list[i], 4);
    printf("address = %s(0x%x)\n", inet_ntoa(addr), ntohl(*(long *)host->h_addr_list[i]));
    i++;
  }

  return 0;
}