#pragma once
#include "storage/ConfigurationStorage.hpp"
#include <memory>
#include <optional>
#include <sw/redis++/redis++.h>

namespace storage::redis {
class RedisClient {
 private:
    std::unique_ptr<sw::redis::Redis> redis{nullptr};

 public:
    RedisClient() = default;
    RedisClient(RedisClient&&) = default;
    RedisClient(const RedisClient&) = delete;
    RedisClient& operator=(RedisClient&&) = default;
    RedisClient& operator=(const RedisClient&) = delete;
    ~RedisClient() = default;

    void init(const storage::DbConfiguration& config);

    std::optional<std::string> get_channel_uri(const std::string& deviceId);
};
}  // namespace storage::redis