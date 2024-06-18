from typing import Generator

import pytest

from myredis import Redis


@pytest.fixture
def redis() -> Generator[Redis, None, None]:
    with Redis(host="localhost") as redis:
        yield redis
