#include "RequestTestPushMessage.hpp"
#include "logger/Logger.hpp"

namespace tcp::messages {
RequestTestPushMessage::RequestTestPushMessage(const nlohmann::json& j) { isValid = from_json(j); }

RequestTestPushMessage::RequestTestPushMessage(std::string&& deviceId) : AbstractMessage(std::string{ACTION}), deviceId(std::move(deviceId)) {}

const std::string& RequestTestPushMessage::get_device_id() const { return deviceId; }

AbstractMessage::MessageType RequestTestPushMessage::get_type() const { return MessageType::REQUEST_TEST_PUSH_MESSAGE; }

bool RequestTestPushMessage::from_json(const nlohmann::json& j) {
    if (!AbstractMessage::from_json(j)) {
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

void RequestTestPushMessage::to_json(nlohmann::json& j) const {
    AbstractMessage::to_json(j);
    j["device_id"] = deviceId;
}

}  // namespace tcp::messages