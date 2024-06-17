#include "./interfaces/redis.h"

char* ping_interactor() {
    return ping_redis();
}

char* echo_interactor(char str[]) {
    return echo_redis(str);
}

char* get_interactor(char key[]) {
    return get_redis(key);
}

char* set_interactor(char key[], char value[], const int lifetime) {
    return set_redis(key, value, lifetime);
}

int wait_interactor(const int replicas_count, const int timeout) {
    return wait_redis(replicas_count, timeout);
}

char* config_get_interactor(char key[]) {
    return config_get_redis(key);
}
