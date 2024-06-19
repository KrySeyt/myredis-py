import asyncio

import pytest

from myredis import AsyncRedis


@pytest.mark.asyncio
async def test_ping(async_redis: AsyncRedis) -> None:
    await async_redis.ping()


@pytest.mark.asyncio
async def test_echo(async_redis: AsyncRedis) -> None:
    assert await async_redis.echo("test_echo") == "test_echo"


@pytest.mark.asyncio
async def test_get_and_set(async_redis: AsyncRedis) -> None:
    await async_redis.set("test_set_key", "test_set_value", lifetime=1)
    assert await async_redis.get("test_set_key") == "test_set_value"

    await asyncio.sleep(1)

    assert await async_redis.get("test_set_key") is None


@pytest.mark.asyncio
async def test_wait(async_redis: AsyncRedis) -> None:
    assert await async_redis.wait(3, 1) == 0


@pytest.mark.asyncio
async def test_config_get(async_redis: AsyncRedis) -> None:
    assert await async_redis.config_get("port") == "6379"
