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
    RedisClient(const RedisClient&) = delete;
    RedisClient& operator=(RedisClient&&) = default;
    RedisClient& operator=(const RedisClient&) = delete;
    ~RedisClient() = default;

 private:
};
}  // namespace storage::redis