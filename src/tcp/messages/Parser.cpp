#include "Parser.hpp"
#include "AbstractMessage.hpp"
#include "RequestTestPushMessage.hpp"
#include "SetChannelUriMessage.hpp"
#include "SetPushAccountsMessage.hpp"
#include "logger/Logger.hpp"
#include <exception>
#include <memory>
#include <nlohmann/json_fwd.hpp>
#include <spdlog/spdlog.h>

namespace tcp::messages {
const std::shared_ptr<AbstractMessage> parse(const std::string& s) {
    try {
        nlohmann::json j = s;
        return parseJson(j);
    } catch (const std::exception& e) {
        SPDLOG_ERROR("Failed to parse received message '{}' as JSON with: {}", s, e.what());
    }
    return nullptr;
}

const std::shared_ptr<AbstractMessage> parseJson(const nlohmann::json& j) {
    std::string action;
    if (!j.contains("action")) {
        SPDLOG_WARN("Missing 'action' field in message.");
        return nullptr;
    }
    j.at("action").get_to(action);
    if (action.empty()) {
        SPDLOG_WARN("Invalid message 'action' value. Expected a non empty string, but received: {}", action);
        return nullptr;
    }

    std::shared_ptr<AbstractMessage> result = nullptr;

    if (action == SetChannelUriMessage::ACTION) {
        result = std::make_shared<SetChannelUriMessage>(j);
    } else if (action == SetPushAccountsMessage::ACTION) {
        result = std::make_shared<SetPushAccountsMessage>(j);
    } else if (action == RequestTestPushMessage::ACTION) {
        result = std::make_shared<RequestTestPushMessage>(j);
    }

    if (result) {
        if (!result->is_valid()) {
            SPDLOG_WARN("Malformed message received: {}", j.dump());
        }
    } else {
        SPDLOG_WARN("Unknown message received: {}", j.dump());
    }
    return result;
}

}  // namespace tcp::messages