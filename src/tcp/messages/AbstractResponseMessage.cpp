#include "AbstractResponseMessage.hpp"
#include "logger/Logger.hpp"
#include "tcp/messages/AbstractMessage.hpp"

namespace tcp::messages {
AbstractResponseMessage::AbstractResponseMessage(int status) : AbstractMessage(std::string{ACTION}), status(status) {}

int AbstractResponseMessage::get_status() const { return status; }

AbstractMessage::MessageType AbstractResponseMessage::get_type() const { return MessageType::ABSTRACT_RESPONSE_MESSAGE; }

bool AbstractResponseMessage::from_json(const nlohmann::json& j) {
    if (!AbstractMessage::from_json(j)) {
        return false;
    }

    if (!j.contains("status")) {
        LOG_WARNING << "Missing 'status' field in message.";
        return false;
    }
    status = j["status"];
    if (status == -1) {
        LOG_WARNING << "Invalid message 'status' value. Expected != -1 , but received: " << status;
        return false;
    }
    return true;
}

void AbstractResponseMessage::to_json(nlohmann::json& j) const {
    AbstractMessage::to_json(j);
    j["status"] = status;
}
}  // namespace tcp::messages