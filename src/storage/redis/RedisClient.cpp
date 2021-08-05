#include "RedisClient.hpp"
#include "logger/Logger.hpp"
#include "utils/CryptoUtils.hpp"
#include <cassert>
#include <chrono>
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
const std::string RedisClient::WNS_TOKEN_KEY = "WNS";
// NOLINTNEXTLINE (cert-err58-cpp)
const std::chrono::days RedisClient::DEFAULT_ENTRY_TIMEOUT = std::chrono::days(7);

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

std::optional<std::string> RedisClient::get_device_id(const std::string& node) {
    std::vector<std::string> keys;
    redis->lrange(node, 0, -1, std::back_inserter(keys));
    if (keys.size() < 2) {
        return std::nullopt;
    }
    return keys[1];
}

std::optional<std::string> RedisClient::get_node_secret(const std::string& node) {
    std::vector<std::string> keys;
    redis->lrange(node, 0, -1, std::back_inserter(keys));
    if (keys.empty()) {
        return std::nullopt;
    }
    return keys[0];
}

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
            // Delete account:
            const std::string& account = keys[i];
            redis->del(account);
            // Delete node:
            std::optional<std::string> node = redis->get(account);
            if (node) {
                redis->del(*node);
            }
        }
        redis->del(deviceId);
    }

    // Replace it with the new one:
    keys.clear();
    keys.push_back(channelUri);
    for (const tcp::messages::SuccessSetPushAccountsMessage::PushAccount& account : accounts) {
        std::string accountId = gen_account(deviceId, account.accountId);
        keys.push_back(accountId);
        // Add account:
        redis->set(accountId, account.node);
        redis->expire(accountId, DEFAULT_ENTRY_TIMEOUT);
        // Add node:
        redis->rpush(account.node, {account.secret, deviceId});
        redis->expire(account.node, DEFAULT_ENTRY_TIMEOUT);
    }
    redis->del(deviceId);
    redis->rpush(deviceId, keys.begin(), keys.end());
    redis->expire(deviceId, DEFAULT_ENTRY_TIMEOUT);
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
    redis->expire(deviceId, DEFAULT_ENTRY_TIMEOUT);
}

void RedisClient::set_wns_token(const std::string& token, const std::string& type, std::chrono::system_clock::time_point expires) {
    std::time_t tt = std::chrono::system_clock::to_time_t(expires);
    redis->set(WNS_TOKEN_KEY, token);

    redis->del(WNS_TOKEN_KEY);
    redis->rpush(WNS_TOKEN_KEY, {token, type, std::to_string(tt)});
    redis->expireat(WNS_TOKEN_KEY, tt);
}

std::optional<std::string> RedisClient::get_wns_token() {
    std::vector<std::string> keys;
    redis->lrange(WNS_TOKEN_KEY, 0, -1, std::back_inserter(keys));
    if (keys.size() != 3) {
        return std::nullopt;
    }
    return keys[0];
}

std::optional<std::string> RedisClient::get_wns_token_type() {
    std::vector<std::string> keys;
    redis->lrange(WNS_TOKEN_KEY, 0, -1, std::back_inserter(keys));
    if (keys.size() != 3) {
        return std::nullopt;
    }
    return keys[1];
}

std::optional<std::chrono::system_clock::time_point> RedisClient::get_wns_token_expire_date() {
    std::vector<std::string> keys;
    redis->lrange(WNS_TOKEN_KEY, 0, -1, std::back_inserter(keys));
    if (keys.size() != 3) {
        return std::nullopt;
    }

    time_t tt = std::strtoll(keys[2].c_str(), nullptr, 10);
    return std::make_optional<std::chrono::system_clock::time_point>(std::chrono::system_clock::from_time_t(tt));
}

void RedisClient::load_push_account(const std::string& deviceId, tcp::messages::SuccessSetPushAccountsMessage::PushAccount* account) {
    const std::string accountId = gen_account(deviceId, account->accountId);
    std::optional<std::string> node = redis->get(accountId);
    if (!node) {
        return;
    }

    std::optional<std::string> secret = get_node_secret(*node);
    if (!secret) {
        return;
    }
    account->node = *node;
    account->secret = *secret;
    account->success = true;
    LOG_DEBUG << "Loaded existing push node and secret.";
}

std::string RedisClient::gen_account(const std::string& deviceId, const std::string& accountId) {
    return utils::hash_sah256(deviceId + '_' + accountId);
}

}  // namespace storage::redis
