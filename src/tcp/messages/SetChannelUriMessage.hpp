#pragma once

#include "AbstractMessage.hpp"
#include <nlohmann/json.hpp>
#include <string>
#include <string_view>

namespace tcp::messages {
class SetChannelUriMessage : public AbstractMessage {
 public:
    static constexpr std::string_view ACTION = "set_channel_uri";

 protected:
    std::string channelUri;
    std::string deviceId;

 public:
    explicit SetChannelUriMessage(const nlohmann::json& j);
    explicit SetChannelUriMessage(std::string&& channelUri, std::string&& deviceId);
    SetChannelUriMessage(SetChannelUriMessage&&) = default;
    SetChannelUriMessage(const SetChannelUriMessage&) = default;
    SetChannelUriMessage& operator=(SetChannelUriMessage&&) = default;
    SetChannelUriMessage& operator=(const SetChannelUriMessage&) = default;
    ~SetChannelUriMessage() override = default;

    [[nodiscard]] const std::string& get_channel_uri() const;
    [[nodiscard]] const std::string& get_device_id() const;
    [[nodiscard]] MessageType get_type() const override;

 protected:
    bool from_json(const nlohmann::json& j) override;

 public:
    void to_json(nlohmann::json& j) const override;
};
}  // namespace tcp::messages