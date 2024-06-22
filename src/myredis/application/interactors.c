#include "./interfaces/redis.h"

int ping_interactor() {
    return ping_redis();
}

int echo_interactor(const char str[]) {
    return echo_redis(str);
}

int get_interactor(const char key[]) {
    return get_redis(key);
}

int set_interactor(const char key[], const char value[], const int lifetime) {
    return set_redis(key, value, lifetime);
}

int wait_interactor(const int replicas_count, const int timeout) {
    return wait_redis(replicas_count, timeout);
}

int config_get_interactor(const char key[]) {
    return config_get_redis(key);
}

void get_response_interactor(const char *out) {
    return get_response_redis(out);
}
