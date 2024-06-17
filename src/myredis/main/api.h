//
// Created by sequew on 6/16/24.
//

#ifndef API_H
#define API_H

int connect_to_redis_server(char host[], const int port);


void disconnect_from_redis_server(const int socket_desc);


char* ping(const int socket_desc);

char* echo(const int socket_desc, char str[]);

char* get(const int socket_desc, char key[]);

char* set(const int socket_desc, char key[], char value[], const int lifetime);

int wait(const int socket_desc, const int replicas_count, const int timeout);

char* config_get(const int socket_desc, char key[]);

#endif //API_H
