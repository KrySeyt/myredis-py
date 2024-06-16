#include "../application/interactors.h"
#include "../external/tcp/redis.h"
#include "api.h"

#include <stdio.h>
#include <stdlib.h>


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


int redis_server_socket_desc;

int connect_to_redis_server(char host[], const int port) {
    return connect_(host, port);
}

void disconnect_from_redis_server(const int socket_desc) {
    close_connection(socket_desc);
}

char* ping(const int socket_desc) {
    redis_server_socket_desc = socket_desc;

    return ping_interactor();
}

char* echo(const int socket_desc, char str[]) {
    redis_server_socket_desc = socket_desc;

    return echo_interactor(str);
}

char* get(const int socket_desc, char key[]) {
    redis_server_socket_desc = socket_desc;

    return get_interactor(key);
}

char* set(const int socket_desc, char key[], char value[], const int lifetime) {
    redis_server_socket_desc = socket_desc;

    return set_interactor(key, value, lifetime);
}

int wait(const int socket_desc, const int replicas_count, const int timeout) {
    redis_server_socket_desc = socket_desc;
    return wait_interactor(replicas_count, timeout);
}
