#include "./interfaces/redis.h"

void ping_interactor() {
    return ping_redis();
}

void echo_interactor(char str[]) {
    return echo_redis(str);
}

void get_interactor(char key[]) {
    return get_redis(key);
}

void set_interactor(char key[], char value[], const int lifetime) {
    return set_redis(key, value, lifetime);
}

void wait_interactor(const int replicas_count, const int timeout) {
    return wait_redis(replicas_count, timeout);
}

void config_get_interactor(char key[]) {
    return config_get_redis(key);
}

char* get_response_interactor(void) {
    return get_response_redis();
}
