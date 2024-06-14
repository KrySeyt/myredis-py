#include <stdio.h>

#include "redis.h"

char* send_command(char* command) {
    return "+PONG\r\n";
}
