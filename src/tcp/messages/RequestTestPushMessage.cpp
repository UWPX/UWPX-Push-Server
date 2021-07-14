#include "RequestTestPushMessage.hpp"
#include "logger/Logger.hpp"

namespace tcp::messages {
RequestTestPushMessage::RequestTestPushMessage(const nlohmann::json& j) : AbstractMessage(from_json(j)) {}

RequestTestPushMessage::RequestTestPushMessage(std::string&& deviceId) : AbstractMessage(std::string{ACTION}), deviceId(std::move(deviceId)) {}

const std::string& RequestTestPushMessage::get_device_id() const { return deviceId; }

AbstractMessage::MessageType RequestTestPushMessage::get_type() const { return MessageType::REQUEST_TEST_PUSH_MESSAGE; }

bool RequestTestPushMessage::from_json(const nlohmann::json& j) {
    if (!AbstractMessage::from_json(j)) {
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

void RequestTestPushMessage::to_json(nlohmann::json& j) const {
    AbstractMessage::to_json(j);
    j["device_id"] = deviceId;
}

}  // namespace tcp::messages