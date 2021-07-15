#include "RedisClient.hpp"
#include "logger/Logger.hpp"
#include <cassert>
#include <memory>
#include <optional>
#include <string>
#include <spdlog/spdlog.h>
#include <sw/redis++/connection.h>
#include <sw/redis++/redis++.h>
#include <sw/redis++/redis.h>

namespace storage::redis {
void RedisClient::init(const storage::DbConfiguration& config) {
    assert(!redis);
    SPDLOG_INFO("Initializing Redis connection...");
    redis = std::make_unique<sw::redis::Redis>(config.url);
    SPDLOG_INFO("Redis connection initialized...");
}

std::optional<std::string> RedisClient::get_channel_uri(const std::string& deviceId) {
    sw::redis::OptionalString result = redis->get(deviceId);
    if (result) {
        return std::make_optional<std::string>(*result);
    }
    return std::nullopt;
}
}  // namespace storage::redis