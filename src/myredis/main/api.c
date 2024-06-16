#include "../application/interactors.h"

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


char *redis_server_host;
int redis_server_port;


char* ping(char host[], const int port) {
    printf("%c\n", 49);
    redis_server_host = host;
    redis_server_port = port;

    return ping_interactor();
}

char* echo(char host[], const int port, char str[]) {
    redis_server_host = host;
    redis_server_port = port;

    return echo_interactor(str);
}

char* get(char host[], const int port, char key[]) {
    redis_server_host = host;
    redis_server_port = port;

    return get_interactor(key);
}

char* set(char host[], const int port, char key[], char value[]) {
    redis_server_host = host;
    redis_server_port = port;

    return set_interactor(key, value);
}