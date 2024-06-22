//
// Created by sequew on 6/16/24.
//

#ifndef REDIS_H
#define REDIS_H

#define CONNECTION_REFUSED_ERROR -1
#define COMMAND_SENDING_ERROR -2
#define UNKNOWN_SERVER_RESPONSE_ERROR -3

int connect_(const char host[], const int port);

void close_connection(const int socket_desc);

#endif //REDIS_H
