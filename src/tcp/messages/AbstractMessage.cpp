#include "AbstractMessage.hpp"
#include "logger/Logger.hpp"
#include <cassert>
#include <nlohmann/json_fwd.hpp>

namespace tcp::messages {
AbstractMessage::AbstractMessage(std::string&& action) : version(VERSION), action(std::move(action)) {}

bool AbstractMessage::is_valid() const { return isValid; }

int AbstractMessage::get_version() const { return version; }

AbstractMessage::MessageType AbstractMessage::get_type() const { return MessageType::ABSTRACT_MESSAGE; }

const std::string& AbstractMessage::get_action() const { return action; }

bool AbstractMessage::from_json(const nlohmann::json& j) {
    if (!j.contains("version")) {
        LOG_WARNING << "Missing 'version' field in message.";
        return false;
    }
    version = j["version"];
    if (version != VERSION) {
        LOG_WARNING << "Invalid message 'version' value. Expected " << VERSION << ", but received: " << action;
        return false;
    }

    if (!j.contains("action")) {
        LOG_WARNING << "Missing 'action' field in message.";
        return false;
    }
    action = j["action"];
    if (action.empty()) {
        LOG_WARNING << "Invalid message 'action' value. Expected a non empty string, but received: " << action;
        return false;
    }
    return true;
}

void AbstractMessage::to_json(nlohmann::json& j) const {
    assert(isValid);
    j["version"] = version;
    j["action"] = action;
}
}  // namespace tcp::messages