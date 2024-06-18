__all__ = (
    "Redis",
)

import socket
import ctypes
from typing import Self
from myasync import Await, IOType, Coroutine

Seconds = float


class Redis:
    def __init__(self, host: str, port: int = 6379) -> None:
        socket.socket()
        self.host = "127.0.0.1" if host == "localhost" else host
        self.port = port

        self._redis_c_lib = ctypes.CDLL("./cmake-build-debug/libmyredis_py.so")

        self._redis_c_lib.ping.restype = ctypes.c_char_p
        self._redis_c_lib.echo.restype = ctypes.c_char_p
        self._redis_c_lib.get.restype = ctypes.c_char_p
        self._redis_c_lib.set.restype = ctypes.c_char_p
        self._redis_c_lib.wait.restype = ctypes.c_int
        self._redis_c_lib.config_get.restype = ctypes.c_char_p
        self._redis_c_lib.connect_to_redis_server.restype = ctypes.c_int

        # TODO: is it OK about abstractions and dependency rule?
        self._redis_server_socket_descriptor = self._connect()

    def get(self, key: str) -> str | None:
        response = self._redis_c_lib.get(
            self._redis_server_socket_descriptor,
            key.encode("utf-8"),
        )

        return response.decode("utf-8") if response else response

    def set(self, key: str, value: str, lifetime: Seconds | None = None) -> None:
        response = self._redis_c_lib.set(
            self._redis_server_socket_descriptor,
            key.encode("utf-8"),
            value.encode("utf-8"),
            int(lifetime * 1000) if lifetime is not None else -1,
        )

        return response.decode("utf-8")

    def echo(self, value: str) -> str:
        response = self._redis_c_lib.echo(
            self._redis_server_socket_descriptor,
            value.encode("utf-8"),
        )
        return response.decode("utf-8")

    def ping(self) -> None:
        self._redis_c_lib.ping(
            self._redis_server_socket_descriptor,
        )

    def wait(self, replicas_count: int, timeout: Seconds) -> None:
        return self._redis_c_lib.wait(
            self._redis_server_socket_descriptor,
            replicas_count,
            timeout * 1000,
        )

    def config_get(self, key: str) -> str | None:
        response = self._redis_c_lib.config_get(
            self._redis_server_socket_descriptor,
            key.encode("utf-8"),
        )

        return response.decode("utf-8") if response else response

    def close(self) -> None:
        self._redis_c_lib.disconnect_from_redis_server(
            self._redis_server_socket_descriptor,
        )

    def __enter__(self) -> Self:
        return self

    def __exit__(self, exc_type, exc_val, exc_tb) -> None:
        self.close()

    def _connect(self) -> int:
        return self._redis_c_lib.connect_to_redis_server(
            self.host.encode("utf-8"),
            self.port,
        )
