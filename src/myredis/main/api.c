#include "../application/interactors.h"
#include "../external/tcp/redis.h"
#include "api.h"

#include <stdbool.h>
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

char* get_response(const int socket_desc) {
    redis_server_socket_desc = socket_desc;

    return get_response_interactor();
}

void send_ping_request(const int socket_desc) {
    redis_server_socket_desc = socket_desc;

    return ping_interactor();
}

void send_echo_request(const int socket_desc, char str[]) {
    redis_server_socket_desc = socket_desc;

    return echo_interactor(str);
}

void send_get_request(const int socket_desc, char key[]) {
    redis_server_socket_desc = socket_desc;

    return get_interactor(key);
}

void send_set_request(const int socket_desc, char key[], char value[], const int lifetime) {
    redis_server_socket_desc = socket_desc;
    return set_interactor(key, value, lifetime);
}

void send_wait_request(const int socket_desc, const int replicas_count, const int timeout) {
    redis_server_socket_desc = socket_desc;

    return wait_interactor(replicas_count, timeout);
}

void send_config_get_request(const int socket_desc, char key[]) {
    redis_server_socket_desc = socket_desc;

    return config_get_interactor(key);
}
