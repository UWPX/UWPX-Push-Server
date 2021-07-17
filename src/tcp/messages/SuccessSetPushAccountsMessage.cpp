#include "SuccessSetPushAccountsMessage.hpp"
#include "logger/Logger.hpp"
#include "tcp/messages/SuccessResponseMessage.hpp"
#include "utils/CryptoUtils.hpp"
#include <nlohmann/json_fwd.hpp>
#include <optional>
#include <utility>

namespace tcp::messages {
SuccessSetPushAccountsMessage::SuccessSetPushAccountsMessage(const nlohmann::json& j) { isValid = from_json(j); }

SuccessSetPushAccountsMessage::SuccessSetPushAccountsMessage(std::vector<PushAccount>&& accounts, std::string&& pushBareJid) : accounts(std::move(accounts)), pushBareJid(std::move(pushBareJid)) {}

const std::vector<SuccessSetPushAccountsMessage::PushAccount>& SuccessSetPushAccountsMessage::get_accounts() const { return accounts; }

const std::string& SuccessSetPushAccountsMessage::get_push_bare_jid() const { return pushBareJid; }

AbstractMessage::MessageType SuccessSetPushAccountsMessage::get_type() const { return MessageType::SUCCESS_SET_PUSH_ACCOUNT_MESSAGE; }

bool SuccessSetPushAccountsMessage::from_json(const nlohmann::json& j) {
    if (!SuccessResponseMessage::from_json(j)) {
        return false;
    }

    if (!j.contains("accounts")) {
        SPDLOG_WARN("Missing 'accounts' field in message.");
        return false;
    }

    for (const auto& accountJson : j["accounts"]) {
        std::optional<PushAccount> account = PushAccount::from_json(accountJson);
        if (!account) {
            return false;
        }
        accounts.push_back(std::move(*account));
    }

    if (accounts.empty()) {
        SPDLOG_WARN("Invalid message 'accounts' value. Expected a non empty array.");
        return false;
    }

    if (!j.contains("push_bare_jid")) {
        SPDLOG_WARN("Missing 'push_bare_jid' field in message.");
        return false;
    }
    pushBareJid = j["push_bare_jid"];
    if (pushBareJid.empty()) {
        SPDLOG_WARN("Invalid message 'push_bare_jid' value. Expected a non empty string, but received: {}", pushBareJid);
        return false;
    }
    return true;
}

void SuccessSetPushAccountsMessage::to_json(nlohmann::json& j) const {
    SuccessResponseMessage::to_json(j);
    j["push_bare_jid"] = pushBareJid;

    nlohmann::json accountsJson = nlohmann::json::array();
    for (const PushAccount& account : accounts) {
        nlohmann::json accountJson;
        account.to_json(accountJson);
        accountsJson.push_back(std::move(accountJson));
    }
    j["accounts"] = std::move(accountsJson);
}

SuccessSetPushAccountsMessage::PushAccount::PushAccount(std::string&& bareJid, std::string&& node, std::string&& secret) : bareJid(std::move(bareJid)), node(std::move(node)), secret(std::move(secret)) {}

std::optional<SuccessSetPushAccountsMessage::PushAccount> SuccessSetPushAccountsMessage::PushAccount::from_json(const nlohmann::json& j) {
    std::string bareJid;
    if (!j.contains("bare_jid")) {
        SPDLOG_WARN("Missing 'bare_jid' field in message.");
        return std::nullopt;
    }
    j.at("bare_jid").get_to(bareJid);
    if (bareJid.empty()) {
        SPDLOG_WARN("Invalid message 'bare_jid' value. Expected a non empty string, but received: {}", bareJid);
        return std::nullopt;
    }

    std::string node;
    if (!j.contains("node")) {
        SPDLOG_WARN("Missing 'node' field in message.");
        return std::nullopt;
    }
    j.at("node").get_to(node);
    if (node.empty()) {
        SPDLOG_WARN("Invalid message 'node' value. Expected a non empty string, but received: {}", node);
        return std::nullopt;
    }

    std::string secret;
    if (!j.contains("secret")) {
        SPDLOG_WARN("Missing 'secret' field in message.");
        return std::nullopt;
    }
    j.at("secret").get_to(secret);
    if (secret.empty()) {
        SPDLOG_WARN("Invalid message 'secret' value. Expected a non empty string, but received: {}", secret);
        return std::nullopt;
    }
    return std::make_optional<PushAccount>(std::move(bareJid), std::move(node), std::move(secret));
}

void SuccessSetPushAccountsMessage::PushAccount::to_json(nlohmann::json& j) const {
    j["bare_jid"] = bareJid;
    j["node"] = node;
    j["secret"] = secret;
}

SuccessSetPushAccountsMessage::PushAccount SuccessSetPushAccountsMessage::PushAccount::create(const std::string& /*deviceId*/, const std::string& bareJid) {
    std::string node = utils::url_safe_random_token(15);
    std::string secret = utils::secure_random_password(15);
    return PushAccount(std::string{bareJid}, std::move(node), std::move(secret));
}

}  // namespace tcp::messages