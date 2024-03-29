#pragma once
#include "storage/ConfigurationStorage.hpp"
#include "tcp/messages/SuccessSetPushAccountsMessage.hpp"
#include "utils/date.hpp"
#include <chrono>
#include <memory>
#include <optional>
#include <string>
#include <vector>

/**
 * Forward declaration to prevent having to link public against this lib.
 **/
namespace sw::redis {
class Redis;
}  // namespace sw::redis

namespace storage::redis {
class RedisClient {
 private:
    static const std::string WNS_TOKEN_KEY;
    static const std::chrono::seconds DEFAULT_ENTRY_TIMEOUT;
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
    std::optional<std::string> get_version(const std::string& deviceId);
    std::optional<std::string> get_device_id(const std::string& node);
    std::vector<std::string> get_push_nodes(const std::string& deviceId);
    std::optional<std::string> get_node_secret(const std::string& node);
    std::optional<std::string> get_account_id(const std::string& node);

    std::optional<std::string> get_wns_token();
    std::optional<std::string> get_wns_token_type();
    std::optional<std::chrono::system_clock::time_point> get_wns_token_expire_date();

    void set_channel_uri(const std::string& deviceId, const std::string& channelUri);
    void set_push_accounts(const std::string& deviceId, const std::string& channelUri, const std::string& version, const std::vector<tcp::messages::SuccessSetPushAccountsMessage::PushAccount>& accounts);

    void set_wns_token(const std::string& token, const std::string& type, std::chrono::system_clock::time_point expires);

    void load_push_account(const std::string& deviceId, tcp::messages::SuccessSetPushAccountsMessage::PushAccount* account);

 private:
    static std::string gen_account(const std::string& deviceId, const std::string& accountId);
};
}  // namespace storage::redis