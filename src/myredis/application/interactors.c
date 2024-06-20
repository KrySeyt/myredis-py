#include "./interfaces/redis.h"

int ping_interactor() {
    return ping_redis();
}

int echo_interactor(char str[]) {
    return echo_redis(str);
}

int get_interactor(char key[]) {
    return get_redis(key);
}

int set_interactor(char key[], char value[], const int lifetime) {
    return set_redis(key, value, lifetime);
}

int wait_interactor(const int replicas_count, const int timeout) {
    return wait_redis(replicas_count, timeout);
}

int config_get_interactor(char key[]) {
    return config_get_redis(key);
}

char* get_response_interactor(void) {
    return get_response_redis();
}
