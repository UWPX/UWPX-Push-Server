#include "RedisClient.hpp"
#include "logger/Logger.hpp"
#include <cassert>
#include <memory>
#include <optional>
#include <string>
#include <sw/redis++/connection.h>
#include <sw/redis++/redis++.h>
#include <sw/redis++/redis.h>

namespace storage::redis {
RedisClient::RedisClient(const storage::DbConfiguration& config) : url(config.url) {}

RedisClient::~RedisClient() = default;

void RedisClient::init() {
    assert(!redis);
    LOG_INFO << "Initializing Redis connection...";
    redis = std::make_unique<sw::redis::Redis>(url);
    LOG_INFO << "Redis connection initialized...";
}

std::optional<std::string> RedisClient::get_channel_uri(const std::string& deviceId) {
    return redis->get(deviceId);
}

void RedisClient::set_push_accounts(const std::string& channelUri, const std::vector<std::string>& accounts) {
    redis->del(channelUri);
    redis->rpush(channelUri, accounts.begin(), accounts.end());
}
}  // namespace storage::redis