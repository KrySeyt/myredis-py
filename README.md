# myredis-py
Python client library for [myredis](https://github.com/KrySeyt/myredis) written in C with Python wrapper. Compatible with original Redis

Why did I choose C and use workarounds for `asyncio` and [myasync](https://github.com/KrySeyt/myasync) for socket operations in C? Just for practice and fun. In any case, no one but me will use this library.

# Setup
- Create venv
```shell
python -m venv .venv 
```

- Activate venv
```shell
. .venv/bin/activate 
```

- Install `myredis-py`
```shell
pip install myredis-py@git+https://github.com/KrySeyt/myredis-py.git
```

# Example
```python
import asyncio

from myredis import AsyncRedis

async def main() -> None:
    async with AsyncRedis(host="localhost", port=6379) as redis:
        await redis.set("foo", "bar", lifetime=1.5)
        print(await redis.get("foo"))

if __name__ == "__main__":
    asyncio.run(main())

```
