#include "redis.h"

char* ping_interactor() {
    return ping_redis();
}

char* echo_interactor(char str[]) {
    return echo_redis(str);
}

char* get_interactor(char key[]) {
    return get_redis(key);
}

char* set_interactor(char key[], char value[]) {
    return set_redis(key, value);
}
