#pragma once

#include "AbstractMessage.hpp"
#include <nlohmann/json.hpp>
#include <string>
#include <string_view>
#include <vector>

namespace tcp::messages {
class SetPushAccountsMessage : public AbstractMessage {
 public:
    static constexpr std::string_view ACTION = "set_push_accounts";

 protected:
    std::vector<std::string> accounts;
    std::string deviceId;

 public:
    explicit SetPushAccountsMessage(const nlohmann::json& j);
    explicit SetPushAccountsMessage(std::vector<std::string>&& accounts, std::string&& deviceId);
    SetPushAccountsMessage(SetPushAccountsMessage&&) = default;
    SetPushAccountsMessage(const SetPushAccountsMessage&) = default;
    SetPushAccountsMessage& operator=(SetPushAccountsMessage&&) = default;
    SetPushAccountsMessage& operator=(const SetPushAccountsMessage&) = default;
    ~SetPushAccountsMessage() override = default;

    [[nodiscard]] const std::vector<std::string>& get_accounts() const;
    [[nodiscard]] const std::string& get_device_id() const;
    [[nodiscard]] MessageType get_type() const override;

 protected:
    bool from_json(const nlohmann::json& j) override;

 public:
    void to_json(nlohmann::json& j) const override;
};
}  // namespace tcp::messages