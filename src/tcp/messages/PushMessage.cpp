#include "PushMessage.hpp"
#include "logger/Logger.hpp"

namespace tcp::messages {
PushMessage::PushMessage(const nlohmann::json& j) {
    // NOLINTNEXTLINE (clang-analyzer-optin.cplusplus.VirtualCall) Not relevant in this case
    isValid = from_json(j);
}

PushMessage::PushMessage(std::string&& accountId, int messageCount, int pendingSubscriptionCount) : AbstractMessage(std::string{ACTION}),
                                                                                                    accountId(std::move(accountId)),
                                                                                                    messageCount(messageCount),
                                                                                                    pendingSubscriptionCount(pendingSubscriptionCount) {}

const std::string& PushMessage::get_account_id() const {
    return accountId;
}

int PushMessage::get_message_count() const {
    return messageCount;
}

int PushMessage::get_pending_subscription_count() const {
    return pendingSubscriptionCount;
}

AbstractMessage::MessageType PushMessage::get_type() const {
    return MessageType::PUSH;
}

bool PushMessage::from_json(const nlohmann::json& j) {
    if (!AbstractMessage::from_json(j)) {
        return false;
    }

    if (!j.contains("account_id")) {
        LOG_WARNING << "Missing 'account_id' field in message.";
        return false;
    }
    accountId = j["account_id"];
    if (accountId.empty()) {
        LOG_WARNING << "Invalid message 'account_id' value. Expected a non empty string, but received: " << accountId;
        return false;
    }

    if (!j.contains("message_count")) {
        LOG_WARNING << "Missing 'message_count' field in message.";
        return false;
    }
    messageCount = j["message_count"];

    if (!j.contains("pending_subscription_count")) {
        LOG_WARNING << "Missing 'pending_subscription_count' field in message.";
        return false;
    }
    pendingSubscriptionCount = j["pending_subscription_count"];
    return true;
}

void PushMessage::to_json(nlohmann::json& j) const {
    AbstractMessage::to_json(j);
    j["account_id"] = accountId;
    j["message_count"] = messageCount;
    j["pending_subscription_count"] = pendingSubscriptionCount;
}

}  // namespace tcp::messages