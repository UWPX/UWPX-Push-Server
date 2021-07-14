#pragma once

#include "SuccessResponseMessage.hpp"
#include <nlohmann/json.hpp>
#include <optional>
#include <string>
#include <string_view>
#include <vector>

namespace tcp::messages {
class SuccessSetPushAccountsMessage : public SuccessResponseMessage {
 public:
    struct PushAccount {
        std::string bareJid;
        std::string node;
        std::string secret;

        PushAccount(std::string&& bareJid, std::string&& node, std::string&& secret);

        static std::optional<PushAccount> from_json(const nlohmann::json& j);
        void to_json(nlohmann::json& j) const;
    };

 protected:
    std::vector<PushAccount> accounts;
    std::string pushBareJid;

 public:
    explicit SuccessSetPushAccountsMessage(const nlohmann::json& j);
    explicit SuccessSetPushAccountsMessage(std::vector<PushAccount>&& accounts, std::string&& pushBareJid);
    SuccessSetPushAccountsMessage(SuccessSetPushAccountsMessage&&) = default;
    SuccessSetPushAccountsMessage(const SuccessSetPushAccountsMessage&) = default;
    SuccessSetPushAccountsMessage& operator=(SuccessSetPushAccountsMessage&&) = default;
    SuccessSetPushAccountsMessage& operator=(const SuccessSetPushAccountsMessage&) = default;
    ~SuccessSetPushAccountsMessage() override = default;

    [[nodiscard]] const std::vector<PushAccount>& get_accounts() const;
    [[nodiscard]] const std::string& get_push_bare_jid() const;
    [[nodiscard]] MessageType get_type() const override;

 protected:
    bool from_json(const nlohmann::json& j) override;
    void to_json(nlohmann::json& j) const override;
};
}  // namespace tcp::messages