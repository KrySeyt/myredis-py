#include "../adapters/redis.h"

char* ping_interactor() {
    return ping_redis();
}

char* echo_interactor(char str[]) {
    return echo_redis(str);
}
