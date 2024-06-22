//
// Created by sequew on 6/16/24.
//

#ifndef REDIS_H
#define REDIS_H

extern const int CONNECTION_ERROR;

extern const int COMMAND_SENDING_ERROR;

extern const int UNKNOWN_SERVER_RESPONSE_ERROR;

int connect_(const char host[], const int port);

void close_connection(const int socket_desc);

#endif //REDIS_H
