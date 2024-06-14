#include "redis.h"
#include "./interfaces.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char* ping_redis() {
    char command[] = "+PING\r\n";
    return send_command(command);
}

char* echo_redis(char str[]) {
    int str_len = strlen(str);
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
    return send_command(command);
}
