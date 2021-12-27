#pragma once

#include "AbstractMessage.hpp"
#include <nlohmann/json.hpp>
#include <string>
#include <string_view>

namespace tcp::messages {
class PushMessage : public AbstractMessage {
 public:
    static constexpr std::string_view ACTION = "push";

 protected:
    std::string accountId;
    int messageCount = 0;
    int pendingSubscriptionCount = 0;

 public:
    explicit PushMessage(const nlohmann::json& j);
    PushMessage(std::string&& accountId, int messageCount, int pendingSubscriptionCount);
    PushMessage(PushMessage&&) = default;
    PushMessage(const PushMessage&) = default;
    PushMessage& operator=(PushMessage&&) = default;
    PushMessage& operator=(const PushMessage&) = default;
    ~PushMessage() override = default;

    [[nodiscard]] const std::string& get_account_id() const;
    [[nodiscard]] int get_message_count() const;
    [[nodiscard]] int get_pending_subscription_count() const;
    [[nodiscard]] MessageType get_type() const override;

 protected:
    bool from_json(const nlohmann::json& j) override;

 public:
    void to_json(nlohmann::json& j) const override;
};
}  // namespace tcp::messages