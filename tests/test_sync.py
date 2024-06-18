import time

from myredis import Redis


def test_ping(redis: Redis) -> None:
    redis.ping()


def test_echo(redis: Redis) -> None:
    assert redis.echo("test_echo") == "test_echo"


def test_get_and_set(redis: Redis) -> None:
    redis.set("test_set_key", "test_set_value", lifetime=1)
    assert redis.get("test_set_key") == "test_set_value"

    time.sleep(1)

    assert redis.get("test_set_key") is None


def test_wait(redis: Redis) -> None:
    assert redis.wait(3, 1) == 0


def test_config_get(redis: Redis) -> None:
    assert redis.config_get("port") == "6379"
