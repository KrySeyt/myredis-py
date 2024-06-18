#ifndef REDIS_H
#define REDIS_H

void ping_redis();

void echo_redis(char str[]);

void get_redis(char key[]);

void set_redis(char key[], char value[], const int lifetime);

void wait_redis(const int replicas_count, const int timeout);

void config_get_redis(char key[]);

char* get_response_redis(void);

#endif //REDIS_H
