__all__ = (
    "Redis",
)

import socket
import ctypes
from typing import Self, Any
from myasync import Await, IOType, Coroutine

Seconds = float


class Redis:
    def __init__(self, host: str, port: int = 6379) -> None:
        socket.socket()
        self.host = "127.0.0.1" if host == "localhost" else host
        self.port = port

        self._redis_c_lib = ctypes.CDLL("./cmake-build-debug/libmyredis_py.so")

        self._redis_c_lib.get_response.restype = ctypes.c_char_p
        self._redis_c_lib.connect_to_redis_server.restype = ctypes.c_int

        # TODO: is it OK about abstractions and dependency rule?
        self._redis_server_socket_descriptor = self._connect()

    def get(self, key: str) -> str | None:
        self._redis_c_lib.send_get_request(
            self._redis_server_socket_descriptor,
            key.encode("utf-8"),
        )

        response = self._redis_c_lib.get_response(
            self._redis_server_socket_descriptor
        )

        return response.decode("utf-8") if response else response

    def set(self, key: str, value: str, lifetime: Seconds | None = None) -> None:
        self._redis_c_lib.send_set_request(
            self._redis_server_socket_descriptor,
            key.encode("utf-8"),
            value.encode("utf-8"),
            int(lifetime * 1000) if lifetime is not None else -1,
        )

        response = self._redis_c_lib.get_response(
            self._redis_server_socket_descriptor
        )

        return response.decode("utf-8")

    def echo(self, value: str) -> str:
        self._redis_c_lib.send_echo_request(
            self._redis_server_socket_descriptor,
            value.encode("utf-8"),
        )

        response = self._redis_c_lib.get_response(
            self._redis_server_socket_descriptor
        )

        return response.decode("utf-8")

    def ping(self) -> None:
        self._redis_c_lib.send_ping_request(
            self._redis_server_socket_descriptor,
        )

        self._redis_c_lib.get_response(
            self._redis_server_socket_descriptor
        )

    def wait(self, replicas_count: int, timeout: Seconds) -> int:
        self._redis_c_lib.send_wait_request(
            self._redis_server_socket_descriptor,
            replicas_count,
            timeout * 1000,
        )

        response = self._redis_c_lib.get_response(
            self._redis_server_socket_descriptor
        )

        return int(response)

    def config_get(self, key: str) -> str | None:
        self._redis_c_lib.send_config_get_request(
            self._redis_server_socket_descriptor,
            key.encode("utf-8"),
        )

        response = self._redis_c_lib.get_response(
            self._redis_server_socket_descriptor
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


class MyAsyncRedis:
    def __init__(self, host: str, port: int = 6379) -> None:
        socket.socket()
        self.host = "127.0.0.1" if host == "localhost" else host
        self.port = port

        self._redis_c_lib = ctypes.CDLL("./cmake-build-debug/libmyredis_py.so")

        self._redis_c_lib.get_response.restype = ctypes.c_char_p

        self._redis_c_lib.connect_to_redis_server.restype = ctypes.c_int

        # TODO: is it OK about abstractions and dependency rule?
        redis_server_socket_descriptor = self._connect()
        self._redis_server_socket = socket.socket(fileno=redis_server_socket_descriptor)

    def get(self, key: str) -> Coroutine[str | None]:
        yield Await(self._redis_server_socket, IOType.OUTPUT)

        self._redis_c_lib.send_get_request(
            self._redis_server_socket.fileno(),
            key.encode("utf-8"),
        )

        response = yield from self._get_response()

        return response.decode("utf-8") if response else response

    def set(self, key: str, value: str, lifetime: Seconds | None = None) -> Coroutine[None]:
        yield Await(self._redis_server_socket, IOType.OUTPUT)

        self._redis_c_lib.send_set_request(
            self._redis_server_socket.fileno(),
            key.encode("utf-8"),
            value.encode("utf-8"),
            int(lifetime * 1000) if lifetime is not None else -1,
        )

        yield from self._get_response()

    def echo(self, value: str) -> Coroutine[str]:
        yield Await(self._redis_server_socket, IOType.OUTPUT)

        self._redis_c_lib.send_echo_request(
            self._redis_server_socket.fileno(),
            value.encode("utf-8"),
        )

        response = yield from self._get_response()

        return response.decode("utf-8")

    def ping(self) -> Coroutine[None]:
        yield Await(self._redis_server_socket, IOType.OUTPUT)

        self._redis_c_lib.send_ping_request(
            self._redis_server_socket.fileno(),
        )

        yield from self._get_response()

    def wait(self, replicas_count: int, timeout: Seconds) -> Coroutine[int]:
        yield Await(self._redis_server_socket, IOType.OUTPUT)

        self._redis_c_lib.send_wait_request(
            self._redis_server_socket.fileno(),
            replicas_count,
            int(timeout * 1000),
        )

        response = yield from self._get_response()

        return int(response)

    def config_get(self, key: str) -> Coroutine[str | None]:
        yield Await(self._redis_server_socket, IOType.OUTPUT)

        self._redis_c_lib.send_config_get_request(
            self._redis_server_socket.fileno(),
            key.encode("utf-8"),
        )

        response = yield from self._get_response()

        return response.decode("utf-8") if response else response

    def close(self) -> None:
        self._redis_c_lib.disconnect_from_redis_server(
            self._redis_server_socket.fileno(),
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

    def _get_response(self) -> Coroutine[bytes | None]:
        yield Await(self._redis_server_socket, IOType.INPUT)

        return self._redis_c_lib.get_response(
            self._redis_server_socket.fileno(),
        )
