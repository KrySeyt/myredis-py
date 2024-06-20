__all__ = (
    "Redis",
)

import asyncio
import selectors
import socket
import ctypes
from types import TracebackType
from typing import Self, Any, cast
from myasync import Await, IOType, Coroutine

Seconds = float


class CommandSendingError(Exception):
    pass


class UnknownServerResponseError(ValueError):
    pass


class Redis:
    def __init__(self, host: str, port: int = 6379) -> None:
        self.host = "127.0.0.1" if host == "localhost" else host
        self.port = port

        self._redis_c_lib = ctypes.CDLL("./cmake-build-debug/libmyredis_py.so")

        self._redis_c_lib.get_response.restype = ctypes.c_char_p
        self._redis_c_lib.connect_to_redis_server.restype = ctypes.c_int

        self._connection_error_code = ctypes.c_int.in_dll(self._redis_c_lib, "CONNECTION_ERROR").value
        self._sending_command_error_code = ctypes.c_int.in_dll(self._redis_c_lib, "COMMAND_SENDING_ERROR").value
        self._unknown_server_response_error_code = ctypes.c_int.in_dll(
            self._redis_c_lib,
            "UNKNOWN_SERVER_RESPONSE_ERROR",
        ).value

        # TODO: is it OK about abstractions and dependency rule?
        self._redis_server_socket_descriptor = self._connect()

    def get(self, key: str) -> str | None:
        request_result = self._redis_c_lib.send_get_request(
            self._redis_server_socket_descriptor,
            key.encode("utf-8"),
        )
        self._check_request_result(request_result)

        response = self._get_response()
        return response.decode("utf-8") if isinstance(response, bytes) else response

    def set(self, key: str, value: str, lifetime: Seconds | None = None) -> None:
        request_result = self._redis_c_lib.send_set_request(
            self._redis_server_socket_descriptor,
            key.encode("utf-8"),
            value.encode("utf-8"),
            int(lifetime * 1000) if lifetime is not None else -1,
        )
        self._check_request_result(request_result)

        self._get_response()

    def echo(self, value: str) -> str:
        request_result = self._redis_c_lib.send_echo_request(
            self._redis_server_socket_descriptor,
            value.encode("utf-8"),
        )
        self._check_request_result(request_result)

        response = self._get_response()
        assert response is not None
        return response.decode("utf-8")

    def ping(self) -> None:
        request_result = self._redis_c_lib.send_ping_request(
            self._redis_server_socket_descriptor,
        )
        self._check_request_result(request_result)

        self._get_response()

    def wait(self, replicas_count: int, timeout: Seconds) -> int:
        request_result = self._redis_c_lib.send_wait_request(
            self._redis_server_socket_descriptor,
            replicas_count,
            timeout * 1000,
        )
        self._check_request_result(request_result)

        response = self._get_response()
        assert response is not None
        return int(response)

    def config_get(self, key: str) -> str | None:
        request_result = self._redis_c_lib.send_config_get_request(
            self._redis_server_socket_descriptor,
            key.encode("utf-8"),
        )
        self._check_request_result(request_result)

        response = self._get_response()
        return response.decode("utf-8") if isinstance(response, bytes) else response

    def close(self) -> None:
        self._redis_c_lib.disconnect_from_redis_server(
            self._redis_server_socket_descriptor,
        )

    def __enter__(self) -> Self:
        return self

    def __exit__(
            self,
            exc_type: type[BaseException] | None,
            exc_val: BaseException | None,
            exc_tb: TracebackType
    ) -> None:

        self.close()

    def _connect(self) -> int:
        result = self._redis_c_lib.connect_to_redis_server(
            self.host.encode("utf-8"),
            self.port,
        )

        if result == self._connection_error_code:
            raise ConnectionError()

        socket_descriptor = result
        assert isinstance(socket_descriptor, int)
        return socket_descriptor

    def _get_response(self) -> bytes | None:
        response = self._redis_c_lib.get_response(
            self._redis_server_socket_descriptor
        )
        self._check_response(response)

        assert response is None or isinstance(response, bytes)
        return response

    def _check_request_result(self, result: int | None) -> None:
        if result == self._sending_command_error_code:
            raise CommandSendingError

    def _check_response(self, response: bytes | int | None) -> None:
        if response == self._unknown_server_response_error_code:
            raise UnknownServerResponseError


class MyAsyncRedis:
    def __init__(self, host: str, port: int = 6379) -> None:
        socket.socket()
        self.host = "127.0.0.1" if host == "localhost" else host
        self.port = port

        self._redis_c_lib = ctypes.CDLL("./cmake-build-debug/libmyredis_py.so")

        self._redis_c_lib.get_response.restype = ctypes.c_char_p

        self._redis_c_lib.connect_to_redis_server.restype = ctypes.c_int

        self._connection_error_code = ctypes.c_int.in_dll(self._redis_c_lib, "CONNECTION_ERROR").value
        self._sending_command_error_code = ctypes.c_int.in_dll(self._redis_c_lib, "SENDING_COMMAND_ERROR").value
        self._unknown_server_response_error_code = ctypes.c_int.in_dll(
            self._redis_c_lib,
            "UNKNOWN_SERVER_RESPONSE_ERROR",
        ).value

        # TODO: is it OK about abstractions and dependency rule?
        redis_server_socket_descriptor = self._connect()
        self._redis_server_socket = socket.socket(fileno=redis_server_socket_descriptor)

    def get(self, key: str) -> Coroutine[str | None]:
        yield Await(self._redis_server_socket, IOType.OUTPUT)

        request_result = self._redis_c_lib.send_get_request(
            self._redis_server_socket.fileno(),
            key.encode("utf-8"),
        )
        self._check_request_result(request_result)

        response = yield from self._get_response()
        return response.decode("utf-8") if isinstance(response, bytes) else response

    def set(self, key: str, value: str, lifetime: Seconds | None = None) -> Coroutine[None]:
        yield Await(self._redis_server_socket, IOType.OUTPUT)

        request_result = self._redis_c_lib.send_set_request(
            self._redis_server_socket.fileno(),
            key.encode("utf-8"),
            value.encode("utf-8"),
            int(lifetime * 1000) if lifetime is not None else -1,
        )
        self._check_request_result(request_result)

        yield from self._get_response()

    def echo(self, value: str) -> Coroutine[str]:
        yield Await(self._redis_server_socket, IOType.OUTPUT)

        request_result = self._redis_c_lib.send_echo_request(
            self._redis_server_socket.fileno(),
            value.encode("utf-8"),
        )
        self._check_request_result(request_result)

        response = yield from self._get_response()
        assert response is not None
        return response.decode("utf-8")

    def ping(self) -> Coroutine[None]:
        yield Await(self._redis_server_socket, IOType.OUTPUT)

        request_result = self._redis_c_lib.send_ping_request(
            self._redis_server_socket.fileno(),
        )
        self._check_request_result(request_result)

        yield from self._get_response()

    def wait(self, replicas_count: int, timeout: Seconds) -> Coroutine[int]:
        yield Await(self._redis_server_socket, IOType.OUTPUT)

        request_result = self._redis_c_lib.send_wait_request(
            self._redis_server_socket.fileno(),
            replicas_count,
            int(timeout * 1000),
        )
        self._check_request_result(request_result)

        response = yield from self._get_response()
        assert response is not None
        return int(response)

    def config_get(self, key: str) -> Coroutine[str | None]:
        yield Await(self._redis_server_socket, IOType.OUTPUT)

        request_result = self._redis_c_lib.send_config_get_request(
            self._redis_server_socket.fileno(),
            key.encode("utf-8"),
        )
        self._check_request_result(request_result)

        response = yield from self._get_response()
        return response.decode("utf-8") if isinstance(response, bytes) else response

    def close(self) -> None:
        self._redis_c_lib.disconnect_from_redis_server(
            self._redis_server_socket.fileno(),
        )

    def __enter__(self) -> Self:
        return self

    def __exit__(
            self,
            exc_type: type[BaseException] | None,
            exc_val: BaseException | None,
            exc_tb: TracebackType
    ) -> None:

        self.close()

    def _connect(self) -> int:
        result = self._redis_c_lib.connect_to_redis_server(
            self.host.encode("utf-8"),
            self.port,
        )

        if result == self._connection_error_code:
            raise ConnectionError()

        socket_descriptor = result
        assert isinstance(socket_descriptor, int)
        return socket_descriptor

    def _check_response(self, response: bytes | int | None) -> None:
        if response == self._unknown_server_response_error_code:
            raise UnknownServerResponseError

    def _check_request_result(self, result: bytes | int | None) -> None:
        if result == self._sending_command_error_code:
            raise CommandSendingError

    def _get_response(self) -> Coroutine[bytes | None]:
        yield Await(self._redis_server_socket, IOType.INPUT)

        response = self._redis_c_lib.get_response(
            self._redis_server_socket.fileno(),
        )
        self._check_response(response)
        assert response is None or isinstance(response, bytes)

        return response


class AsyncRedis:
    def __init__(self, host: str, port: int = 6379) -> None:
        socket.socket()
        self.host = "127.0.0.1" if host == "localhost" else host
        self.port = port

        self._redis_c_lib = ctypes.CDLL("./cmake-build-debug/libmyredis_py.so")

        self._redis_c_lib.get_response.restype = ctypes.c_char_p

        self._redis_c_lib.connect_to_redis_server.restype = ctypes.c_int

        self._connection_error_code = ctypes.c_int.in_dll(self._redis_c_lib, "CONNECTION_ERROR").value
        self._sending_command_error_code = ctypes.c_int.in_dll(self._redis_c_lib, "COMMAND_SENDING_ERROR").value
        self._unknown_server_response_error_code = ctypes.c_int.in_dll(
            self._redis_c_lib,
            "UNKNOWN_SERVER_RESPONSE_ERROR",
        ).value

        redis_server_socket_descriptor = self._connect()
        self._redis_server_socket = socket.socket(fileno=redis_server_socket_descriptor)
        self._selector: selectors.BaseSelector = type(
            asyncio.get_event_loop()._selector  # type: ignore [attr-defined]
        )()
        self._selector.register(self._redis_server_socket, selectors.EVENT_READ + selectors.EVENT_WRITE)

    async def get(self, key: str) -> str | None:
        await self._wait_event_ready(selectors.EVENT_WRITE)

        request_result = self._redis_c_lib.send_get_request(
            self._redis_server_socket.fileno(),
            key.encode("utf-8"),
        )
        self._check_request_result(request_result)

        response = await self._get_response()
        return response.decode("utf-8") if isinstance(response, bytes) else response

    async def set(self, key: str, value: str, lifetime: Seconds | None = None) -> None:
        await self._wait_event_ready(selectors.EVENT_WRITE)

        request_result = self._redis_c_lib.send_set_request(
            self._redis_server_socket.fileno(),
            key.encode("utf-8"),
            value.encode("utf-8"),
            int(lifetime * 1000) if lifetime is not None else -1,
        )
        self._check_request_result(request_result)

        await self._get_response()

    async def echo(self, value: str) -> str:
        await self._wait_event_ready(selectors.EVENT_WRITE)

        request_result = self._redis_c_lib.send_echo_request(
            self._redis_server_socket.fileno(),
            value.encode("utf-8"),
        )
        self._check_request_result(request_result)

        response = await self._get_response()
        assert response is not None
        return response.decode("utf-8")

    async def _wait_event_ready(self, event: int) -> None:
        wait = True
        while wait:
            await asyncio.sleep(0)

            r = self._selector.select()

            if not r:
                continue

            for i in r:
                selector_key, ready_event = i
                if ready_event in (event, selectors.EVENT_READ + selectors.EVENT_WRITE):
                    wait = False
                    break

    async def ping(self) -> None:
        await self._wait_event_ready(selectors.EVENT_WRITE)

        request_result = self._redis_c_lib.send_ping_request(
            self._redis_server_socket.fileno(),
        )
        self._check_request_result(request_result)

        await self._get_response()

    async def wait(self, replicas_count: int, timeout: Seconds) -> int:
        await self._wait_event_ready(selectors.EVENT_WRITE)

        request_result = self._redis_c_lib.send_wait_request(
            self._redis_server_socket.fileno(),
            replicas_count,
            int(timeout * 1000),
        )
        self._check_request_result(request_result)

        response = await self._get_response()
        assert response is not None
        return int(response)

    async def config_get(self, key: str) -> str | None:
        await self._wait_event_ready(selectors.EVENT_WRITE)

        request_result = self._redis_c_lib.send_config_get_request(
            self._redis_server_socket.fileno(),
            key.encode("utf-8"),
        )
        self._check_request_result(request_result)

        response = await self._get_response()
        return response.decode("utf-8") if isinstance(response, bytes) else response

    async def close(self) -> None:
        self._redis_c_lib.disconnect_from_redis_server(
            self._redis_server_socket.fileno(),
        )

    async def __aenter__(self) -> Self:
        return self

    async def __aexit__(
            self,
            exc_type: type[BaseException] | None,
            exc_val: BaseException | None,
            exc_tb: TracebackType,
    ) -> None:
        await self.close()

    def _connect(self) -> int:
        result = self._redis_c_lib.connect_to_redis_server(
            self.host.encode("utf-8"),
            self.port,
        )

        if result == self._connection_error_code:
            raise ConnectionError()

        socket_descriptor = result
        assert isinstance(socket_descriptor, int)
        return socket_descriptor

    async def _get_response(self) -> bytes | None:
        await self._wait_event_ready(selectors.EVENT_READ)

        response = self._redis_c_lib.get_response(
            self._redis_server_socket.fileno(),
        )
        self._check_response(response)

        assert response is None or isinstance(response, bytes)
        return response

    def _check_response(self, response: bytes | int | None) -> None:
        if response == self._unknown_server_response_error_code:
            raise UnknownServerResponseError

    def _check_request_result(self, result: bytes | int | None) -> None:
        if result == self._sending_command_error_code:
            raise CommandSendingError
