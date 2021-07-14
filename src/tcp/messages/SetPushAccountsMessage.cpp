#include "SetPushAccountsMessage.hpp"
#include "logger/Logger.hpp"
#include <nlohmann/json_fwd.hpp>

namespace tcp::messages {
SetPushAccountsMessage::SetPushAccountsMessage(const nlohmann::json& j) : AbstractMessage(from_json(j)) {}

SetPushAccountsMessage::SetPushAccountsMessage(std::vector<std::string>&& accounts, std::string&& deviceId) : AbstractMessage(std::string{ACTION}), accounts(std::move(accounts)), deviceId(std::move(deviceId)) {}

const std::vector<std::string>& SetPushAccountsMessage::get_accounts() const { return accounts; }

const std::string& SetPushAccountsMessage::get_device_id() const { return deviceId; }

AbstractMessage::MessageType SetPushAccountsMessage::get_type() const { return MessageType::SET_PUSH_ACCOUNT_MESSAGE; }

bool SetPushAccountsMessage::from_json(const nlohmann::json& j) {
    if (!AbstractMessage::from_json(j)) {
        return false;
    }

    if (!j.contains("accounts")) {
        SPDLOG_WARN("Missing 'accounts' field in message.");
        return false;
    }

    for (const auto& accountJson : j["accounts"]) {
        if (!accountJson.contains("bare_jid")) {
            SPDLOG_WARN("Missing 'bare_jid' field in message.");
            return false;
        }
        std::string jid;
        j.at("bare_jid").get_to(jid);
        if (jid.empty()) {
            SPDLOG_WARN("Invalid message 'bare_jid' value. Expected a non empty string, but received: {}", jid);
            return false;
        }
        accounts.push_back(std::move(jid));
    }

    if (accounts.empty()) {
        SPDLOG_WARN("Invalid message 'accounts' value. Expected a non empty array.");
        return false;
    }

    if (!j.contains("device_id")) {
        SPDLOG_WARN("Missing 'device_id' field in message.");
        return false;
    }
    j.at("device_id").get_to(deviceId);
    if (deviceId.empty()) {
        SPDLOG_WARN("Invalid message 'device_id' value. Expected a non empty string, but received: {}", deviceId);
        return false;
    }
    return true;
}

void SetPushAccountsMessage::to_json(nlohmann::json& j) const {
    AbstractMessage::to_json(j);
    j["device_id"] = deviceId;

    nlohmann::json accountsJson = nlohmann::json::array();
    for (const std::string& account : accounts) {
        accountsJson.push_back({{"bare_jid", account}});
    }
    j["accounts"] = std::move(accountsJson);
}

}  // namespace tcp::messages