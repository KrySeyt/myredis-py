#ifndef REDIS_H
#define REDIS_H

char* ping_redis();

char* echo_redis(char str[]);

char* get_redis(char key[]);

char* set_redis(char key[], char value[], const int lifetime);

int wait_redis(const int replicas_count, const int timeout);

char* config_get_redis(char key[]);

#endif //REDIS_H
