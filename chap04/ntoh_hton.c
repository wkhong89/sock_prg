#include <stdio.h>
#include <arpa/inet.h>

int main(void) 
{
  short hostData = 0x1234;
  short netData;
  char *ch;

  printf("original data: 0x1234\n");

  ch = (char *) &hostData;

  if (ch[0] == 0x12 && ch[1] == 0x34) {
    printf("host(big-endian): ");
  } else {
    printf("host(little-endian): ");
  }

  printf("0x%x%x\n", ch[0], ch[1]);

  netData = htons(hostData);

  ch = (char *)&netData;
  printf("network data(big-endian): 0x%x%x\n", ch[0], ch[1]);

  return 0;
}