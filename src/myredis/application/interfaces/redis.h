#ifndef REDIS_H
#define REDIS_H

int ping_redis();

int echo_redis(const char str[]);

int get_redis(const char key[]);

int set_redis(const char key[], const char value[], const int lifetime);

int wait_redis(const int replicas_count, const int timeout);

int config_get_redis(const char key[]);

void get_response_redis(const char *out);

#endif //REDIS_H
