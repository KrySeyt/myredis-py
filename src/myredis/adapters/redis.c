#include "redis.h"
#include "../external/tcp/redis.h"

char* ping_redis() {
    char command[] = "+PING\r\n";
    return send_command(command);
}
