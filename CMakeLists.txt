cmake_minimum_required(VERSION 3.28)
project(myredis C)

set(CMAKE_C_STANDARD 11)

add_library(myredis_py SHARED
        src/myredis/application/interactors.h
        src/myredis/application/interactors.c
        src/myredis/adapters/redis.h
        src/myredis/adapters/redis.c
        src/myredis/external/tcp/redis.h
        src/myredis/external/tcp/redis.c
        src/myredis/main/api.h
        src/myredis/main/api.c)
