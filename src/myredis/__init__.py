__all__ = (
    "Redis",
    "AsyncRedis",
    "MyAsyncRedis",
    "CommandSendingError",
    "UnknownServerResponseError"
)

import asyncio
import selectors
import socket
from types import TracebackType
from typing import Self

import _myredis
from _myredis import CommandSendingError, UnknownServerResponseError
from myasync import Await, IOType, Coroutine

Seconds = float


class Redis:
    def __init__(self, host: str, port: int = 6379) -> None:
        self.host = "127.0.0.1" if host == "localhost" else host
        self.port = port

        # TODO: is it OK about abstractions and dependency rule?
        self._redis_server_socket_descriptor = self._connect()

    def get(self, key: str) -> str | None:
        _myredis.send_get_request(
            self._redis_server_socket_descriptor,
            key,
        )

        response = self._get_response()
        return response.decode("utf-8") if isinstance(response, bytes) else response

    def set(self, key: str, value: str, lifetime: Seconds | None = None) -> None:
        _myredis.send_set_request(
            self._redis_server_socket_descriptor,
            key,
            value,
            int(lifetime * 1000) if lifetime is not None else -1,
        )

        self._get_response()

    def echo(self, value: str) -> str:
        _myredis.send_echo_request(
            self._redis_server_socket_descriptor,
            value,
        )

        response = self._get_response()
        assert response is not None
        return response.decode("utf-8")

    def ping(self) -> None:
        _myredis.send_ping_request(
            self._redis_server_socket_descriptor,
        )

        self._get_response()

    def wait(self, replicas_count: int, timeout: Seconds) -> int:
        _myredis.send_wait_request(
            self._redis_server_socket_descriptor,
            replicas_count,
            int(timeout * 1000),
        )

        response = self._get_response()
        assert response is not None
        return int(response)

    def config_get(self, key: str) -> str | None:
        _myredis.send_config_get_request(
            self._redis_server_socket_descriptor,
            key,
        )

        response = self._get_response()
        return response.decode("utf-8") if isinstance(response, bytes) else response

    def close(self) -> None:
        _myredis.disconnect_from_redis_server(
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
        result = _myredis.connect_to_redis_server(
            self.host,
            self.port,
        )

        socket_descriptor = result
        assert isinstance(socket_descriptor, int)
        return socket_descriptor

    def _get_response(self) -> bytes | None:
        response = _myredis.get_response(
            self._redis_server_socket_descriptor,
        )

        assert response is None or isinstance(response, bytes)
        return response if response else None


class MyAsyncRedis:
    def __init__(self, host: str, port: int = 6379) -> None:
        socket.socket()
        self.host = "127.0.0.1" if host == "localhost" else host
        self.port = port

        # TODO: is it OK about abstractions and dependency rule?
        redis_server_socket_descriptor = self._connect()
        self._redis_server_socket = socket.socket(fileno=redis_server_socket_descriptor)

    def get(self, key: str) -> Coroutine[str | None]:
        yield Await(self._redis_server_socket, IOType.OUTPUT)

        _myredis.send_get_request(
            self._redis_server_socket.fileno(),
            key,
        )

        response = yield from self._get_response()
        return response.decode("utf-8") if isinstance(response, bytes) else response

    def set(self, key: str, value: str, lifetime: Seconds | None = None) -> Coroutine[None]:
        yield Await(self._redis_server_socket, IOType.OUTPUT)

        _myredis.send_set_request(
            self._redis_server_socket.fileno(),
            key,
            value,
            int(lifetime * 1000) if lifetime is not None else -1,
        )

        yield from self._get_response()

    def echo(self, value: str) -> Coroutine[str]:
        yield Await(self._redis_server_socket, IOType.OUTPUT)

        _myredis.send_echo_request(
            self._redis_server_socket.fileno(),
            value,
        )

        response = yield from self._get_response()
        assert response is not None
        return response.decode("utf-8")

    def ping(self) -> Coroutine[None]:
        yield Await(self._redis_server_socket, IOType.OUTPUT)

        _myredis.send_ping_request(
            self._redis_server_socket.fileno(),
        )

        yield from self._get_response()

    def wait(self, replicas_count: int, timeout: Seconds) -> Coroutine[int]:
        yield Await(self._redis_server_socket, IOType.OUTPUT)

        _myredis.send_wait_request(
            self._redis_server_socket.fileno(),
            replicas_count,
            int(timeout * 1000),
        )

        response = yield from self._get_response()
        assert response is not None
        return int(response)

    def config_get(self, key: str) -> Coroutine[str | None]:
        yield Await(self._redis_server_socket, IOType.OUTPUT)

        _myredis.send_config_get_request(
            self._redis_server_socket.fileno(),
            key,
        )

        response = yield from self._get_response()
        return response.decode("utf-8") if isinstance(response, bytes) else response

    def close(self) -> None:
        _myredis.disconnect_from_redis_server(
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
        result = _myredis.connect_to_redis_server(
            self.host,
            self.port,
        )

        socket_descriptor = result
        assert isinstance(socket_descriptor, int)
        return socket_descriptor

    def _get_response(self) -> Coroutine[bytes | None]:
        yield Await(self._redis_server_socket, IOType.INPUT)

        response = _myredis.get_response(
            self._redis_server_socket.fileno(),
        )

        assert response is None or isinstance(response, bytes)
        return response if response else None


class AsyncRedis:
    def __init__(self, host: str, port: int = 6379) -> None:
        socket.socket()
        self.host = "127.0.0.1" if host == "localhost" else host
        self.port = port

        redis_server_socket_descriptor = self._connect()
        self._redis_server_socket = socket.socket(fileno=redis_server_socket_descriptor)
        self._selector: selectors.BaseSelector = type(
            asyncio.get_event_loop()._selector  # type: ignore [attr-defined]
        )()
        self._selector.register(self._redis_server_socket, selectors.EVENT_READ + selectors.EVENT_WRITE)

    async def get(self, key: str) -> str | None:
        await self._wait_event_ready(selectors.EVENT_WRITE)

        _myredis.send_get_request(
            self._redis_server_socket.fileno(),
            key,
        )

        response = await self._get_response()
        return response.decode("utf-8") if isinstance(response, bytes) else response

    async def set(self, key: str, value: str, lifetime: Seconds | None = None) -> None:
        await self._wait_event_ready(selectors.EVENT_WRITE)

        _myredis.send_set_request(
            self._redis_server_socket.fileno(),
            key,
            value,
            int(lifetime * 1000) if lifetime is not None else -1,
        )

        await self._get_response()

    async def echo(self, value: str) -> str:
        await self._wait_event_ready(selectors.EVENT_WRITE)

        _myredis.send_echo_request(
            self._redis_server_socket.fileno(),
            value,
        )

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

        _myredis.send_ping_request(
            self._redis_server_socket.fileno(),
        )

        await self._get_response()

    async def wait(self, replicas_count: int, timeout: Seconds) -> int:
        await self._wait_event_ready(selectors.EVENT_WRITE)

        _myredis.send_wait_request(
            self._redis_server_socket.fileno(),
            replicas_count,
            int(timeout * 1000),
        )

        response = await self._get_response()
        assert response is not None
        return int(response)

    async def config_get(self, key: str) -> str | None:
        await self._wait_event_ready(selectors.EVENT_WRITE)

        _myredis.send_config_get_request(
            self._redis_server_socket.fileno(),
            key,
        )

        response = await self._get_response()
        return response.decode("utf-8") if isinstance(response, bytes) else response

    async def close(self) -> None:
        _myredis.disconnect_from_redis_server(
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
        result = _myredis.connect_to_redis_server(
            self.host,
            self.port,
        )

        socket_descriptor = result
        assert isinstance(socket_descriptor, int)
        return socket_descriptor

    async def _get_response(self) -> bytes | None:
        await self._wait_event_ready(selectors.EVENT_READ)

        response = _myredis.get_response(
            self._redis_server_socket.fileno(),
        )

        assert response is None or isinstance(response, bytes)
        return response if response else None
