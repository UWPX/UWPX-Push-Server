#pragma once
#include "storage/ConfigurationStorage.hpp"
#include <sw/redis++/redis++.h>

namespace storage::redis {
class RedisClient {
 private:
    sw::redis::Redis redis;

 public:
    explicit RedisClient(const storage::DbConfiguration& config);
    RedisClient(RedisClient&&) = default;
    RedisClient(const RedisClient&) = default;
    RedisClient& operator=(RedisClient&&) = default;
    RedisClient& operator=(const RedisClient&) = default;
    ~RedisClient() = default;

 private:
};
}  // namespace storage::redis