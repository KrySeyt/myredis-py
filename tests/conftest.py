from typing import Generator

import pytest
import pytest_asyncio

from myredis import Redis, AsyncRedis


@pytest.fixture
def redis() -> Generator[Redis, None, None]:
    with Redis(host="localhost") as redis:
        yield redis


@pytest_asyncio.fixture
async def async_redis() -> Generator[AsyncRedis, None, None]:
    async with AsyncRedis(host="localhost") as redis:
        yield redis
