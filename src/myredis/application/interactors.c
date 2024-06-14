#include "../adapters/redis.h"

char* ping_interactor() {
    return ping_redis();
}
