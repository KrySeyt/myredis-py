#include "sys/socket.h"
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include <unistd.h>

#include "../../adapters/interfaces/redis.h"
#include "../../application/interfaces/redis.h"
#include "../../adapters/response_parser.h"

static int substr_count(char base_str[], char target[]) {
    char *substr_start = strstr(base_str, target);

    if (substr_start == NULL)
        return 0;

    return 1 + substr_count(substr_start + strlen(target), target);

}

static char* read_from_socket(int socket_desc) {
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
            while ((curr - response + 1) < (atoi(response + 1) + 6) ) {
                curr += recv(socket_desc, curr, 2000, 0);
            }

            *(++curr) = '\0';

            return response;

        case '*':
            curr = response + received_bytes - 1;
            while (substr_count(response, "\r\n") != 5) {
                curr += recv(socket_desc, curr, 2000, 0);
            }
            *(++curr) = '\0';

            return response;

        default:
            printf("Unknown server response: %s\n", response);
            exit(1);
    }

}

int connect_(char redis_server_host[], const int redis_server_port) {
    int socket_desc = socket(AF_INET, SOCK_STREAM, 0);

    if (socket_desc < 0) {
        printf("Unable to create socket");
        exit(1);
    }

    struct sockaddr_in server_addr;

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(redis_server_port);
    server_addr.sin_addr.s_addr = inet_addr(redis_server_host);

    if(connect(socket_desc, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0){
        printf("Unable to connect\n");
        exit(1);
    }

    return socket_desc;
}

void close_connection(const int socket_desc) {
    shutdown(socket_desc, SHUT_RDWR);
    close(socket_desc);
}

void send_command(char command[]) {
    extern int redis_server_socket_desc;

    int r = send(redis_server_socket_desc, command, strlen(command), 0);

    if (r < 0) {
        printf("Unable to send cmd");
        exit(1);
    }
}

char* get_response_redis(void) {
    extern int redis_server_socket_desc;

    char *response = read_from_socket(redis_server_socket_desc);

    return parse_response(response);
}
