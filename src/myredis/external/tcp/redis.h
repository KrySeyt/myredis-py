//
// Created by sequew on 6/16/24.
//

#ifndef REDIS_H
#define REDIS_H

int connect_(char host[], const int port);

void close_connection(const int socket_desc);

#endif //REDIS_H
