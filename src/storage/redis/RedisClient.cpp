#include "RedisClient.hpp"
#include "logger/Logger.hpp"
#include <cassert>
#include <cstdint>
#include <ctime>
#include <memory>
#include <optional>
#include <string>
#include <bits/types/time_t.h>
#include <sw/redis++/connection.h>
#include <sw/redis++/redis++.h>
#include <sw/redis++/redis.h>

namespace storage::redis {

// NOLINTNEXTLINE (cert-err58-cpp)
const std::string RedisClient::WNS_TOKEN_KEY = "WNS_TOKEN";
// NOLINTNEXTLINE (cert-err58-cpp)
const std::string RedisClient::WNS_TOKEN_TYPE_KEY = "WNS_TOKEN_TYPE";
// NOLINTNEXTLINE (cert-err58-cpp)
const std::string RedisClient::WNS_TOKEN_EXPIRES_KEY = "WNS_TOKEN_EXPIRES";

RedisClient::RedisClient(const storage::DbConfiguration& config) : url(config.url) {}

RedisClient::~RedisClient() = default;

void RedisClient::init() {
    assert(!redis);
    LOG_INFO << "Initializing Redis connection...";
    redis = std::make_unique<sw::redis::Redis>(url);
    LOG_INFO << "Redis connection initialized...";
}

std::optional<std::string> RedisClient::get_channel_uri(const std::string& deviceId) { return redis->get(deviceId); }

void RedisClient::set_push_accounts(const std::string& channelUri, const std::vector<std::string>& accounts) {
    redis->del(channelUri);
    redis->rpush(channelUri, accounts.begin(), accounts.end());
}

void RedisClient::set_channel_uri(const std::string& deviceId, const std::string& channelUri) { redis->set(deviceId, channelUri); }

void RedisClient::set_wns_token(const std::string& token) { redis->set(WNS_TOKEN_KEY, token); }

std::optional<std::string> RedisClient::get_wns_token() { return redis->get(WNS_TOKEN_KEY); }

void RedisClient::set_wns_token_type(const std::string& type) { redis->set(WNS_TOKEN_TYPE_KEY, type); }

std::optional<std::string> RedisClient::get_wns_token_type() { return redis->get(WNS_TOKEN_TYPE_KEY); }

void RedisClient::set_wns_token_expire_date(std::chrono::system_clock::time_point expires) {
    std::time_t tt = std::chrono::system_clock::to_time_t(expires);
    redis->set(WNS_TOKEN_EXPIRES_KEY, std::to_string(tt));
}

std::optional<std::chrono::system_clock::time_point> RedisClient::get_wns_token_expire_date() {
    std::optional<std::string> tmp = redis->get(WNS_TOKEN_EXPIRES_KEY);
    if (!tmp) {
        return std::nullopt;
    }
    time_t tt = std::strtoll(tmp->c_str(), nullptr, 10);
    return std::make_optional<std::chrono::system_clock::time_point>(std::chrono::system_clock::from_time_t(tt));
}

}  // namespace storage::redis