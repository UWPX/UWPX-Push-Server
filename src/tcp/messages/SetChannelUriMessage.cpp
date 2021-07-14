#include "SetChannelUriMessage.hpp"
#include "logger/Logger.hpp"

namespace tcp::messages {
SetChannelUriMessage::SetChannelUriMessage(const nlohmann::json& j) : AbstractMessage(from_json(j)) {}

SetChannelUriMessage::SetChannelUriMessage(std::string&& channelUri, std::string&& deviceId) : AbstractMessage(std::string{ACTION}), channelUri(std::move(channelUri)), deviceId(std::move(deviceId)) {}

const std::string& SetChannelUriMessage::get_channel_uri() const { return channelUri; }

const std::string& SetChannelUriMessage::get_device_id() const { return deviceId; }

AbstractMessage::MessageType SetChannelUriMessage::get_type() const { return MessageType::SET_PUSH_ACCOUNT_MESSAGE; }

bool SetChannelUriMessage::from_json(const nlohmann::json& j) {
    if (!AbstractMessage::from_json(j)) {
        return false;
    }

    if (!j.contains("channel_uri")) {
        SPDLOG_WARN("Missing 'channel_uri' field in message.");
        return false;
    }
    j.at("channel_uri").get_to(channelUri);
    if (channelUri.empty()) {
        SPDLOG_WARN("Invalid message 'channel_uri' value. Expected a non empty string, but received: {}", channelUri);
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

void SetChannelUriMessage::to_json(nlohmann::json& j) const {
    AbstractMessage::to_json(j);
    j["device_id"] = deviceId;
    j["channel_uri"] = channelUri;
}

}  // namespace tcp::messages