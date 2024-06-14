__all__ = (
    "Redis",
)

import ctypes
from typing import Any


class Redis:
    def __init__(self, host: str = "localhost", port: int = 6379) -> None:
        self.host = host
        self.port = port
        self._c_lib = ctypes.CDLL("./cmake-build-debug/libmyredis_py.so")
        self._c_lib.ping.restype = ctypes.c_char_p
        self._c_lib.echo.restype = ctypes.c_char_p

    def get(self, key: str) -> Any:
        ...

    def set(self, key: str, value: Any) -> None:
        ...

    def echo(self, value: str) -> str:
        response = self._c_lib.echo(value)
        print(response.decode("utf-8"))

    def ping(self) -> None:
        response = self._c_lib.ping()
        print(response.decode("utf-8"))
