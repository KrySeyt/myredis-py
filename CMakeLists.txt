cmake_minimum_required(VERSION 3.28)
project(myredis C)

set(CMAKE_C_STANDARD 11)

add_library(myredis_py SHARED
        src/myredis/application/interactors.h
        src/myredis/application/interactors.c
        src/myredis/adapters/interfaces/redis.h
        src/myredis/adapters/commands.c
        src/myredis/external/tcp/redis.c
        src/myredis/main/python_api.c
        src/myredis/application/interfaces/redis.h
        src/myredis/adapters/response_parser.h
        src/myredis/adapters/response_parser.c
        src/myredis/external/tcp/redis.h
        src/myredis/main/python_api.h
        src/myredis/external/tcp/errors.c
)

include_directories("/usr/include/python3.12")

