def connect_to_redis_server(host: str, port: int) -> int: ...

def disconnect_from_redis_server(socket_descriptor: int) -> int: ...

def send_ping_request(socket_descriptor: int) -> int: ...

def send_echo_request(socket_descriptor: int, string: str) -> int: ...

def send_get_request(socket_descriptor: int, key: str) -> int: ...

def send_set_request(socket_descriptor: int, key: str, value: str, lifetime: int) -> int: ...

def send_wait_request(socket_descriptor: int, replicas_count: int, timeout: int) -> int: ...

def send_config_get_request(socket_descriptor: int, key: str) -> int: ...

def get_response(socket_descriptor: int) -> str: ...
