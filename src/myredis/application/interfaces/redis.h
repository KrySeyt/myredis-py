#ifndef REDIS_H
#define REDIS_H

char* ping_redis();

char* echo_redis(char str[]);

char* get_redis(char key[]);

char* set_redis(char key[], char value[]);

#endif //REDIS_H
