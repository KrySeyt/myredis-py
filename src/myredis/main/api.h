//
// Created by sequew on 6/16/24.
//

#ifndef API_H
#define API_H

int connect_to_redis_server(char host[], const int port);

void disconnect_from_redis_server(const int socket_desc);

int send_ping_request(const int socket_desc);

int send_echo_request(const int socket_desc, char str[]);

int send_get_request(const int socket_desc, char key[]);

int send_set_request(const int socket_desc, char key[], char value[], const int lifetime);

int send_wait_request(const int socket_desc, const int replicas_count, const int timeout);

int send_config_get_request(const int socket_desc, char key[]);

char* get_redis_response(const int socket_desc);

#endif //API_H
