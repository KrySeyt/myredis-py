#include "redis.h"

#include "sys/socket.h"
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include <unistd.h>

#include "../../adapters/interfaces/redis.h"
#include "../../application/interfaces/redis.h"
#include "redis.h"

#include "../../adapters/response_parser.h"


static void print(char str[]) {
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

static int count_new_line(const char str[]) {
    int str_len = strlen(str);
    int c = 0;
    for (int i = 0; i < str_len; i++)
    {
        if (str[i] == '\n')
            c++;
    }
    return c;
}

static int count_nulls(const char str[]) {
    int str_len = strlen(str);
    int c = 0;
    for (int i = 0; i < str_len; i++)
    {
        if (str[i-2] == '$' && str[i-1] == '-' && str[i] == '1')
            c++;
    }
    return c;
}

static int read_from_socket(const int socket_desc, char* response, const int response_size, const int start_write) {
    static int s = 0;
    const int received_bytes = recv(socket_desc, response + start_write, response_size, 0);
    s += received_bytes;

    if (received_bytes == response_size) {
        return BUFFER_IS_FULL;
    }

    if (!s) {
        return 0;
    }

    char *curr;
    switch (response[0]) {
        case '+': case ':':
            curr = response + s - 1;
            while (*curr != '\n') {
                curr += recv(socket_desc, curr, 2000, 0);
            }
            *(++curr) = '\0';

            s = 0;
            return 0;

        case '$':
            curr = response + s - 1;
            while ((curr - response + 1) < (atoi(response + 1) + 6) ) {
                curr += recv(socket_desc, curr, 2000, 0);
            }

            *(++curr) = '\0';

            s = 0;
            return 0;

        case '*':
            curr = response + s;
            *curr = '\0';
            while (count_new_line(response) != 5 - count_nulls(response)) {
                curr += recv(socket_desc, curr, 2000, 0);
            }
            *(++curr) = '\0';

            s = 0;
            return 0;

        default:
            s = 0;
            return UNKNOWN_SERVER_RESPONSE_ERROR;
    }

}

int connect_(const char redis_server_host[], const int redis_server_port) {
    const int socket_desc = socket(AF_INET, SOCK_STREAM, 0);

    if (socket_desc < 0) {
        return SOCKET_CREATION_ERROR;
    }

    struct sockaddr_in server_addr;

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(redis_server_port);
    server_addr.sin_addr.s_addr = inet_addr(redis_server_host);

    if(connect(socket_desc, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0){
        return CONNECTION_REFUSED_ERROR;
    }

    return socket_desc;
}

void close_connection(const int socket_desc) {
    shutdown(socket_desc, SHUT_RDWR);
    close(socket_desc);
}

int send_command(const char command[]) {
    extern int redis_server_socket_desc;

    const int status = send(redis_server_socket_desc, command, strlen(command), 0);

    if (status < 0) {
        return COMMAND_SENDING_ERROR;
    }
    
    return 0;
}

int get_response_redis(const char *out) {
    extern int redis_server_socket_desc;

    int c = 1;
    char *response = malloc(1000 * c);
    int status = read_from_socket(redis_server_socket_desc, response, 1000, 0);

    while (status == BUFFER_IS_FULL) {
        c++;
        int response_size = 1000 * c;

        char *bigger_response = malloc(response_size);
        memcpy(bigger_response, response, 1000 * (c - 1));
        free(response);

        response = bigger_response;
        status = read_from_socket(redis_server_socket_desc, response, 1000 * c, 1000 * (c - 1) - 1);
    }

    parse_response(response, out);

    free(response);

    return status;
}
