__all__ = (
    "Redis",
)

import ctypes
from typing import Any


class Redis:
    def __init__(self, host: str = "127.0.0.1", port: int = 6379) -> None:
        self.host = host
        self.port = port

        self._c_lib = ctypes.CDLL("./cmake-build-debug/libmyredis_py.so")

        self._c_lib.ping.restype = ctypes.c_char_p
        self._c_lib.echo.restype = ctypes.c_char_p
        self._c_lib.get.restype = ctypes.c_char_p
        self._c_lib.set.restype = ctypes.c_char_p

    def get(self, key: str) -> Any:
        response = self._c_lib.get(
            self.host.encode("utf-8"),
            self.port,
            key.encode("utf-8")
        )

        return response.decode("utf-8")

    def set(self, key: str, value: str) -> None:
        response = self._c_lib.set(
            self.host.encode("utf-8"),
            self.port,
            key.encode("utf-8"),
            value.encode("utf-8")
        )

        return response.decode("utf-8")

    def echo(self, value: str) -> str:
        response = self._c_lib.echo(
            self.host.encode("utf-8"),
            self.port,
            value.encode("utf-8"),
        )
        return response.decode("utf-8")

    def ping(self) -> None:
        self._c_lib.ping(
            self.host.encode("utf-8"),
            self.port,
        )
