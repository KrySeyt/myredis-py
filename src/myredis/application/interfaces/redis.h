#ifndef REDIS_H
#define REDIS_H

int ping_redis();

int echo_redis(char str[]);

int get_redis(char key[]);

int set_redis(char key[], char value[], const int lifetime);

int wait_redis(const int replicas_count, const int timeout);

int config_get_redis(char key[]);

char* get_response_redis(void);

#endif //REDIS_H
