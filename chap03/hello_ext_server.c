#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <netinet/in.h>
#include <sys/socket.h>

#define PORT 9000

char buffer[BUFSIZ] = "hello, world\n";
char rBuffer[BUFSIZ];


int main(void) 
{
    int c_socket, s_socket;
    struct sockaddr_in s_addr, c_addr;
    int len;
    int n;

    if ((s_socket = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("Socket error : ");
        exit(0);
    }

    memset(&s_addr, 0, sizeof(s_addr));
    s_addr.sin_family = AF_INET;
    s_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    s_addr.sin_port = htons(PORT);

    if (bind(s_socket, (struct sockaddr *)&s_addr, sizeof(s_addr)) == -1) {
        perror("Bind error: ");
        exit(0);
    }

    if (listen(s_socket, 5) == -1) {
        perror("Listen error: ");
        exit(0);
    }

    while (1) {
        len = sizeof(c_addr);
        if ((c_socket = accept(s_socket, (struct sockaddr *)&c_addr, &len)) == -1) {
            perror("Accept error: ");
            exit(0);
        }
        printf("Connection Established....\n");

        int length = 0;
        char *temp = rBuffer;

        while ((n = read(c_socket, temp, 1)) > 0) {
            if (*temp == '\r') continue;
            if (*temp == '\n') break;
            if (*temp == '\0') break;

            if (length == BUFSIZ) break;
            temp++;
            length++;
        }

        rBuffer[length] = '\0';

        if (!strcmp(rBuffer, "print")) {
            n = strlen(buffer);
            write(c_socket, buffer, n);
        }

        close(c_socket);
        printf("Connection closed....\n");
    }

    close(s_socket);
}
