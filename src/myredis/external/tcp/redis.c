#include "sys/socket.h"
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include <unistd.h> // read(), write(), close()

#include "../../adapters/interfaces/redis.h"

#include <string.h>

static char host[] = "127.0.0.1";
static int port = 6379;

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

    printf("Connected with server successfully\n");
    printf("CMD:\n%s\n", command);
    int r = send(socket_desc, command, strlen(command), 0);

    if (r < 0) {
        printf("Unable to send cmd");
        exit(1);
    }

    char *response = malloc(1000);
    r = recv(socket_desc, response, sizeof(response), 0);

    if (r < 0) {
        printf("Error while receiving server response");
        exit(1);
    }

    printf("Server response: %s\n", response);

    shutdown(socket_desc, SHUT_RDWR);
    close(socket_desc);

    return response;
}
