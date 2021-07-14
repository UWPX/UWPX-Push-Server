#include "Parser.hpp"
#include "AbstractMessage.hpp"
#include "RequestTestPushMessage.hpp"
#include "SetChannelUriMessage.hpp"
#include "SetPushAccountsMessage.hpp"
#include "logger/Logger.hpp"
#include "tcp/messages/AbstractResponseMessage.hpp"
#include "tcp/messages/ErrorResponseMessage.hpp"
#include "tcp/messages/SuccessResponseMessage.hpp"
#include "tcp/messages/SuccessSetPushAccountsMessage.hpp"
#include <exception>
#include <iostream>
#include <memory>
#include <nlohmann/json_fwd.hpp>
#include <spdlog/spdlog.h>

namespace tcp::messages {
const std::shared_ptr<AbstractMessage> parse(const std::string& s) {
    try {
        nlohmann::json j = nlohmann::json::parse(s);
        return parse_json(j);
    } catch (const nlohmann::json::parse_error& e) {
        SPDLOG_ERROR("Failed to parse received message '{}' as JSON with: {}", s, e.what());
    } catch (const std::exception& e) {
        SPDLOG_ERROR("Failed to evaluate received JSON message '{}' with: {}", s, e.what());
    }
    return nullptr;
}

const std::shared_ptr<AbstractMessage> parse_json(const nlohmann::json& j) {
    if (!j.contains("action")) {
        SPDLOG_WARN("Missing 'action' field in message. {}", j.dump());
        return nullptr;
    }
    std::string action;
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
    } else if (action == AbstractResponseMessage::ACTION) {
        if (!j.contains("status")) {
            SPDLOG_WARN("Missing 'status' field in response message. {}", j.dump());
            return nullptr;
        }
        int status = -1;
        j.at("status").get_to(status);
        if (status == SuccessResponseMessage::STATUS) {
            if (j.contains("accounts")) {
                result = std::make_shared<SuccessSetPushAccountsMessage>(j);
            } else {
                result = std::make_shared<SuccessResponseMessage>(j);
            }
        } else if (status == ErrorResponseMessage::STATUS) {
            result = std::make_shared<ErrorResponseMessage>(j);
        } else {
            SPDLOG_WARN("Invalid message 'action' value. Expected a non empty string, but received: {}", action);
            return nullptr;
        }
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