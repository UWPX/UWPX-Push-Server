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
RedisClient::RedisClient(const storage::DbConfiguration& config) : url(config.url) {}

RedisClient::~RedisClient() = default;

void RedisClient::init() {
    assert(!redis);
    SPDLOG_INFO("Initializing Redis connection...");
    redis = std::make_unique<sw::redis::Redis>(url);
    SPDLOG_INFO("Redis connection initialized...");
}

std::optional<std::string> RedisClient::get_channel_uri(const std::string& deviceId) {
    return redis->get(deviceId);
}
}  // namespace storage::redis