#include "ErrorResponseMessage.hpp"
#include "logger/Logger.hpp"
#include "tcp/messages/AbstractResponseMessage.hpp"

namespace tcp::messages {
ErrorResponseMessage::ErrorResponseMessage(const nlohmann::json& j) {
    // NOLINTNEXTLINE (clang-analyzer-optin.cplusplus.VirtualCall) Not relevant in this case
    isValid = from_json(j);
}

ErrorResponseMessage::ErrorResponseMessage(std::string&& error) : AbstractResponseMessage(STATUS), error(std::move(error)) {}

const std::string& ErrorResponseMessage::get_error() const { return error; }

AbstractMessage::MessageType ErrorResponseMessage::get_type() const { return MessageType::ERROR_RESPONSE_MESSAGE; }

bool ErrorResponseMessage::from_json(const nlohmann::json& j) {
    if (!AbstractResponseMessage::from_json(j)) {
        return false;
    }

    if (!j.contains("error")) {
        LOG_WARNING << "Missing 'error' field in message.";
        return false;
    }
    error = j["error"];
    return true;
}

void ErrorResponseMessage::to_json(nlohmann::json& j) const {
    AbstractResponseMessage::to_json(j);
    j["error"] = error;
}

}  // namespace tcp::messages