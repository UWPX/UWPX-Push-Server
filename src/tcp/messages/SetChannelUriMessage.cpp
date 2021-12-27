#include "SetChannelUriMessage.hpp"
#include "logger/Logger.hpp"

namespace tcp::messages {
SetChannelUriMessage::SetChannelUriMessage(const nlohmann::json& j) {
    // NOLINTNEXTLINE (clang-analyzer-optin.cplusplus.VirtualCall) Not relevant in this case
    isValid = from_json(j);
}

SetChannelUriMessage::SetChannelUriMessage(std::string&& channelUri, std::string&& deviceId) : AbstractMessage(std::string{ACTION}), channelUri(std::move(channelUri)), deviceId(std::move(deviceId)) {}

const std::string& SetChannelUriMessage::get_channel_uri() const { return channelUri; }

const std::string& SetChannelUriMessage::get_device_id() const { return deviceId; }

AbstractMessage::MessageType SetChannelUriMessage::get_type() const { return MessageType::SET_CHANNEL_URI_MESSAGE; }

bool SetChannelUriMessage::from_json(const nlohmann::json& j) {
    if (!AbstractMessage::from_json(j)) {
        return false;
    }

    if (!j.contains("channel_uri")) {
        LOG_WARNING << "Missing 'channel_uri' field in message.";
        return false;
    }
    channelUri = j["channel_uri"];
    if (channelUri.empty()) {
        LOG_WARNING << "Invalid message 'channel_uri' value. Expected a non empty string, but received: " << channelUri;
        return false;
    }

    if (!j.contains("device_id")) {
        LOG_WARNING << "Missing 'device_id' field in message.";
        return false;
    }
    deviceId = j["device_id"];
    if (deviceId.empty()) {
        LOG_WARNING << "Invalid message 'device_id' value. Expected a non empty string, but received: " << deviceId;
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