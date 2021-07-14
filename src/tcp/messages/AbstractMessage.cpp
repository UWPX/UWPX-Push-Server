#include "AbstractMessage.hpp"
#include "logger/Logger.hpp"
#include <cassert>
#include <nlohmann/json_fwd.hpp>
#include <spdlog/spdlog.h>

namespace tcp::messages {
AbstractMessage::AbstractMessage(bool isValid) : isValid(isValid) {}

AbstractMessage::AbstractMessage(std::string&& action) : isValid(true), version(VERSION), action(std::move(action)) {}

bool AbstractMessage::is_valid() const { return isValid; }

int AbstractMessage::get_version() const { return version; }

AbstractMessage::MessageType AbstractMessage::get_type() const { return MessageType::ABSTRACT_MESSAGE; }

const std::string& AbstractMessage::get_action() const { return action; }

bool AbstractMessage::from_json(const nlohmann::json& j) {
    if (!j.contains("version")) {
        SPDLOG_WARN("Missing 'version' field in message.");
        return false;
    }
    j.at("version").get_to(version);
    if (version != VERSION) {
        SPDLOG_WARN("Invalid message 'version' value. Expected {}, but received: {}", VERSION, action);
        return false;
    }

    if (!j.contains("action")) {
        SPDLOG_WARN("Missing 'action' field in message.");
        return false;
    }
    j.at("action").get_to(action);
    if (action.empty()) {
        SPDLOG_WARN("Invalid message 'action' value. Expected a non empty string, but received: {}", action);
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