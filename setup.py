from setuptools import Extension, setup

setup(
    ext_modules=[
        Extension(
            name="_myredis",
            sources=[
                "src/myredis/main/python_api.c",
                "src/myredis/application/interactors.c",
                "src/myredis/adapters/commands.c",
                "src/myredis/adapters/response_parser.c",
                "src/myredis/external/tcp/errors.c",
                "src/myredis/external/tcp/redis.c",
            ],
        ),
    ]
)
