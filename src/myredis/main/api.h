//
// Created by sequew on 6/16/24.
//

#ifndef API_H
#define API_H

int connect_to_redis_server(char host[], const int port);

void disconnect_from_redis_server(const int socket_desc);

void send_ping_request(const int socket_desc);

void send_echo_request(const int socket_desc, char str[]);

void send_get_request(const int socket_desc, char key[]);

void send_set_request(const int socket_desc, char key[], char value[], const int lifetime);

void send_wait_request(const int socket_desc, const int replicas_count, const int timeout);

void send_config_get_request(const int socket_desc, char key[]);

char* get_redis_response(const int socket_desc);

#endif //API_H
