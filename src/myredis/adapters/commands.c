#include "../application/interfaces/redis.h"
#include "./interfaces/redis.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <tgmath.h>
#include <time.h>

int ping_redis() {
    const char command[] = "*1\r\n$4\r\nPING\r\n";
    return send_command(command);
}

int echo_redis(const char str[]) {
    const int str_len = strlen(str);
    char* command = malloc(str_len + 30);
    sprintf(
        command,
        "*2\r\n"
        "$4\r\n"
        "ECHO\r\n"
        "$%d\r\n"
        "%s\r\n",
        str_len,
        str
    );
    int result = send_command(command);
    free(command);
    return result;
}

int get_redis(const char key[]) {
    const int key_len = strlen(key);
    char* command = malloc(key_len + 30);
    sprintf(
        command,
        "*2\r\n"
        "$3\r\n"
        "GET\r\n"
        "$%d\r\n"
        "%s\r\n",
        key_len,
        key
    );
    const int result = send_command(command);
    free(command);
    return result;
}

int set_redis(const char key[], const char value[], const int lifetime) {
    const int key_len = strlen(key);
    const int value_len = strlen(value);
    char* command = malloc(key_len + value_len + 1000);

    if (lifetime == -1)
        sprintf(
            command,
            "*3\r\n"
            "$3\r\n"
            "SET\r\n"
            "$%d\r\n"
            "%s\r\n"
            "$%d\r\n"
            "%s\r\n",
            key_len,
            key,
            value_len,
            value
        );
    else
        sprintf(
            command,
            "*5\r\n"
            "$3\r\n"
            "SET\r\n"
            "$%d\r\n"
            "%s\r\n"
            "$%d\r\n"
            "%s\r\n"
            "$2\r\n"
            "PX\r\n"
            "$%d\r\n"
            "%d\r\n",
            key_len,
            key,
            value_len,
            value,
            (int) log10(abs(lifetime)) + 1,
            lifetime
        );

    const int result = send_command(command);
    free(command);
    return result;
}

int wait_redis(const int replicas_count, const int timeout) {
    char* command = malloc(300);
    sprintf(
        command,
        "*3\r\n"
        "$4\r\n"
        "WAIT\r\n"
        "$%d\r\n"
        "%d\r\n"
        "$%d\r\n"
        "%d\r\n",
        (int) log10(abs(replicas_count)) + 1,
        replicas_count,
        (int) log10(abs(timeout)) + 1,
        timeout
    );
    const int result = send_command(command);
    free(command);
    return result;
}

int config_get_redis(const char key[]) {
    const int key_len = strlen(key);
    char* command = malloc(key_len + 300);
    sprintf(
        command,
        "*3\r\n"
        "$6\r\n"
        "CONFIG\r\n"
        "$3\r\n"
        "GET\r\n"
        "$%d\r\n"
        "%s\r\n",
        key_len,
        key
    );
    const int result = send_command(command);
    free(command);
    return result;
}
