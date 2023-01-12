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
    int len;
    int n;

    char rcvBuffer[BUFSIZ];

    c_socket=socket(AF_INET, SOCK_STREAM, 0);

    memset(&c_addr, 0, sizeof(c_addr));
    c_addr.sin_family = AF_INET;
    c_addr.sin_addr.s_addr = inet_addr(IPADDR);
    c_addr.sin_port = htons(PORT);

    if (connect(c_socket, (struct sockaddr *)&c_addr, sizeof(c_addr)) < 0) {
        perror("Connect error: ");
        close(c_socket);
        exit(0);
    }

    if ((n = read(c_socket, rcvBuffer, sizeof(rcvBuffer))) < 0) {
        perror("Read error: ");
        exit(0);
    }

    rcvBuffer[n] = '\0';
    printf("buffer size: %d filled with %d\n", BUFSIZ, n);
    printf("received Data: %s\n", rcvBuffer);

    close(c_socket);

}
