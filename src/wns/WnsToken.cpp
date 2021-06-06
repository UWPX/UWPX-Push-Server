#include "WnsToken.hpp"
#include "logger/Logger.hpp"
#include <algorithm>
#include <chrono>
#include <cstddef>
#include <memory>
#include <nlohmann/json.hpp>
#include <optional>
#include <string>

namespace wns {
WnsToken::WnsToken(std::string&& type, std::string&& token, std::chrono::system_clock::time_point expires) : type(std::move(type)), token(std::move(token)), expires(expires) {}

bool WnsToken::isValid() {
    // NOLINTNEXTLINE (modernize-use-nullptr)
    return std::chrono::system_clock::now() < expires;
}

std::shared_ptr<WnsToken> WnsToken::fromResponse(const std::string& response) {
    try {
        nlohmann::json js = response;

        // Token type:
        if (!js.contains("token_type")) {
            SPDLOG_ERROR("Failed to parse 'token_type' from '{}'", response);
            return nullptr;
        }
        std::string type;
        js.at("token_type").get_to(type);

        // Token:
        if (!js.contains("access_token")) {
            SPDLOG_ERROR("Failed to parse 'access_token' from '{}'", response);
            return nullptr;
        }
        std::string token;
        js.at("access_token").get_to(token);

        // Expires:
        if (!js.contains("expires_in")) {
            SPDLOG_ERROR("Failed to parse 'expires_in' from '{}'", response);
            return nullptr;
        }
        size_t expiresInSeconds = 0;
        js.at("expires_in").get_to(expiresInSeconds);
        std::chrono::system_clock::time_point expires = getExpiresDate(expiresInSeconds);
        return std::make_shared<WnsToken>(std::move(type), std::move(token), expires);

    } catch (nlohmann::json::parse_error& e) {
        SPDLOG_ERROR("Error parsing WNS token from '{}': {}", response, e.what());
    }
    return nullptr;
}

std::chrono::system_clock::time_point WnsToken::getExpiresDate(size_t expiresInSeconds) {
    return std::chrono::system_clock::now() + std::chrono::seconds(expiresInSeconds);
}
}  // namespace wns