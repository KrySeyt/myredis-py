#include "sys/socket.h"
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include <unistd.h>

#include "../../adapters/interfaces/redis.h"

static char host[] = "127.0.0.1";
static int port = 6379;

void print(char str[]) {
    for (; *str != '\0'; str++) {
        if (*str == '\n') {
            putchar('\\');
            putchar('n');
        }

        else if (*str == '\r') {
            putchar('\\');
            putchar('r');
        }
        else {
            putchar(*str);
        }
    }

    putchar('\n');
}

char* read_from_socket(int socket_desc) {
    char *response = malloc(2000); // TODO: mb memory leak
    int received_bytes = recv(socket_desc, response, 2000, 0);

    if (!received_bytes)
        return "";

    char *curr;
    switch (response[0]) {
        case '+': case ':':
            curr = response + received_bytes - 1;
            while (*curr != '\n') {
                curr += recv(socket_desc, curr, 2000, 0);
            }
            *(++curr) = '\0';

            return response;

        case '$':
            curr = response + received_bytes - 1;
            while (*curr != '\n' && curr - response < 2 && (curr - response) < (response[1] - '0' + 6) ) {
                curr += recv(socket_desc, curr, 2000, 0);
            }
            *(++curr) = '\0';

            return response;

        default:
            printf("Unknown server response: %s\n", response);
            exit(1);
    }

}

char* send_command(char command[]) {
    int socket_desc = socket(AF_INET, SOCK_STREAM, 0);

    if (socket_desc < 0) {
        printf("Unable to create socket");
        exit(1);
    }

    struct sockaddr_in server_addr;

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    server_addr.sin_addr.s_addr = inet_addr(host);

    if(connect(socket_desc, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0){
        printf("Unable to connect\n");
        exit(1);
    }

    int r = send(socket_desc, command, strlen(command), 0);

    if (r < 0) {
        printf("Unable to send cmd");
        exit(1);
    }

    char *response = read_from_socket(socket_desc);

    shutdown(socket_desc, SHUT_RDWR);
    close(socket_desc);

    return response;
}
