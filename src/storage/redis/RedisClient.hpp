#pragma once
#include "storage/ConfigurationStorage.hpp"
#include <memory>
#include <optional>
#include <string>

/**
 * Forward declaration to prevent having to link public against this lib.
 **/
namespace sw::redis {
class Redis;
}  // namespace sw::redis

namespace storage::redis {
class RedisClient {
 private:
    const std::string url;
    std::unique_ptr<sw::redis::Redis> redis{nullptr};

 public:
    explicit RedisClient(const storage::DbConfiguration& config);
    RedisClient(RedisClient&&) = delete;
    RedisClient(const RedisClient&) = delete;
    RedisClient& operator=(RedisClient&&) = delete;
    RedisClient& operator=(const RedisClient&) = delete;
    ~RedisClient();

    void init();

    std::optional<std::string> get_channel_uri(const std::string& deviceId);
};
}  // namespace storage::redis