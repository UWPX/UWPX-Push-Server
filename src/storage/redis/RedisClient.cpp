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

std::optional<std::string> RedisClient::get_channel_uri(const std::string& deviceId) {
    std::vector<std::string> keys;
    redis->lrange(deviceId, 0, -1, std::back_inserter(keys));
    if (keys.empty()) {
        return std::nullopt;
    }
    return keys[0];
}

std::optional<std::string> RedisClient::get_node_secret(const std::string& node) { return redis->get(node); }

std::vector<std::string> RedisClient::get_push_nodes(const std::string& deviceId) {
    std::vector<std::string> nodes;
    std::vector<std::string> keys;
    redis->lrange(deviceId, 0, -1, std::back_inserter(keys));
    for (size_t i = 1; i < keys.size(); i++) {
        std::optional<std::string> node = redis->get(keys[i]);
        assert(node);
        nodes.push_back(*node);
    }
    return nodes;
}

void RedisClient::set_push_accounts(const std::string& deviceId, const std::string& channelUri, const std::vector<tcp::messages::SuccessSetPushAccountsMessage::PushAccount>& accounts) {
    // Remove the existing key structure:
    std::vector<std::string> keys;
    redis->lrange(deviceId, 0, -1, std::back_inserter(keys));
    if (!keys.empty()) {
        for (size_t i = 1; i < keys.size(); i++) {
            redis->del(keys[i]);
        }
        redis->del(deviceId);
    }

    // Replace it with the new one:
    keys.clear();
    keys.push_back(channelUri);
    for (const tcp::messages::SuccessSetPushAccountsMessage::PushAccount& account : accounts) {
        std::string accountId = deviceId + "_" + account.bareJid;  // TODO: hash the bare JID here
        keys.push_back(accountId);
        redis->set(accountId, account.node);
        redis->set(account.node, account.secret);
    }
    redis->del(deviceId);
    redis->rpush(deviceId, keys.begin(), keys.end());
}

void RedisClient::set_channel_uri(const std::string& deviceId, const std::string& channelUri) {
    std::vector<std::string> keys;
    redis->lrange(deviceId, 0, -1, std::back_inserter(keys));
    if (keys.empty()) {
        keys.push_back(channelUri);
    } else {
        keys[0] = channelUri;
        redis->del(deviceId);
    }
    redis->rpush(deviceId, keys.begin(), keys.end());
}

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