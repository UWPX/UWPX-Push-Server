#include "WnsClient.hpp"
#include "logger/Logger.hpp"
#include <cassert>
#include <cstdint>
#include <memory>
#include <optional>
#include <string>
#include <utility>
#include <cpr/api.h>
#include <cpr/body.h>
#include <cpr/cprtypes.h>
#include <cpr/payload.h>
#include <cpr/response.h>
#include <cpr/ssl_options.h>

namespace wns {
WnsClient::WnsClient(const storage::WnsConfiguration& config) : packetSid(config.packetId), clientSecret(config.clientSecret) {}

bool WnsClient::is_token_valid() {
    return token && token->is_valid();
}

bool WnsClient::request_new_token() {
    cpr::Payload payload{
        {"grant_type", "client_credentials"},
        {"scope", "notify.windows.com"},
        {"client_id", packetSid},
        {"client_secret", clientSecret}};
    cpr::Url url{"https://login.live.com/accesstoken.srf"};
    cpr::Response response = cpr::Post(url, payload);
    if (response.status_code != 200) {
        if (response.error.code == cpr::ErrorCode::OK) {
            LOG_ERROR << "Requesting a new token failed. Status code: " << response.status_code << "\nResponse: " << response.text;
        } else {
            LOG_ERROR << "Requesting a new token failed. Status code: " << response.status_code << "\nError: " << response.error.message;
        }
        return false;
    }
    token = WnsToken::from_response(response.text);
    if (token) {
        store_token_in_db();
        LOG_INFO << "Successfully requested a new WNS token.";
        LOG_DEBUG << "Token: '" << token->token << "', Type: '" << token->type << "'";
        return true;
    }
    return false;
}

void WnsClient::set_redis_client(storage::redis::RedisClient* redisClient) {
    this->redisClient = redisClient;
}

void WnsClient::load_token_from_db() {
    assert(redisClient);

    std::optional<std::string> token = redisClient->get_wns_token();
    if (!token) {
        return;
    }
    std::optional<std::string> type = redisClient->get_wns_token_type();
    if (!type) {
        return;
    }
    std::optional<std::chrono::system_clock::time_point> expires = redisClient->get_wns_token_expire_date();
    if (!expires) {
        return;
    }

    this->token = std::make_shared<WnsToken>(std::move(*type), std::move(*token), *expires);
    LOG_INFO << "Successfully loaded the WNS token from the DB.";
    LOG_DEBUG << "Token: '" << this->token->token << "', Type: '" << this->token->type << "'";
}

void WnsClient::store_token_in_db() {
    assert(redisClient);
    redisClient->set_wns_token(token->token);
    redisClient->set_wns_token_type(token->type);
    redisClient->set_wns_token_expire_date(token->expires);
}

bool WnsClient::send_raw_notification(const std::string& channelUri, const std::string&& content) {
    LOG_DEBUG << "Sending raw notification...";
    if (is_token_valid()) {
        LOG_INFO << "WNS token expired. requesting a new one...";
        if (!request_new_token()) {
            LOG_ERROR << "Failed to send raw notification. Requesting a new WNS token failed.";
            return false;
        }
    }

    // https://docs.microsoft.com/en-us/previous-versions/windows/apps/hh465435(v=win.10)#request-parameters
    cpr::Header header{{"X-WNS-Type", "wns/raw"},
                       {"X-WNS-RequestForStatus", "true"},
                       {"Content-Type", "application/octet-stream"},
                       {"Authorization", token->to_auth_string()}};

    cpr::Url url{channelUri};
    cpr::Body body{content};
    for (uint8_t retryCount = 0; retryCount < 3; retryCount++) {
        cpr::Response response = cpr::Post(url, header, body);
        if (response.error.code != cpr::ErrorCode::OK) {
            LOG_ERROR << "Requesting a new token failed. Status code: " << response.status_code << "\nError: " << response.error.message;
            continue;
        }
        ResponseCodeAction result = handle_response_code(response.status_code, response.text);
        if (result == ResponseCodeAction::SUCCESS) {
            return true;
        }
        if (result == ResponseCodeAction::RETRY) {
            continue;
        }
        return false;
    }
    return false;
}

WnsClient::ResponseCodeAction WnsClient::handle_response_code(int64_t statusCode, const std::string& text) {
    switch (statusCode) {
        case StatusCodes::STATUS_CODE_OK:
            LOG_INFO << "Successfully send raw notification.";
            return ResponseCodeAction::SUCCESS;

        case StatusCodes::STATUS_CODE_BAD_REQUEST:
            LOG_ERROR << "Sending a raw notification failed. Malformed headers.";
            return ResponseCodeAction::ERROR;

        case StatusCodes::STATUS_CODE_UNAUTHORIZED:
            LOG_ERROR << "Sending a raw notification failed. WNS token invalid.";
            LOG_INFO << "Requesting a new WNS token...";
            if (request_new_token()) {
                return ResponseCodeAction::RETRY;
            }
            return ResponseCodeAction::ERROR;

        case StatusCodes::STATUS_CODE_FORBIDDEN:
            LOG_ERROR << "Sending a raw notification failed. WNS token does not match the app credentials.";
            return ResponseCodeAction::ERROR;

        case StatusCodes::STATUS_CODE_NOT_FOUND:
            LOG_ERROR << "Sending a raw notification failed. Channel URI is not valid.";
            return ResponseCodeAction::ERROR;

        case StatusCodes::STATUS_CODE_METHOD_NOT_ALLOWED:
            LOG_ERROR << "Sending a raw notification failed. HTTP POST should be used.";
            return ResponseCodeAction::ERROR;

        case StatusCodes::STATUS_CODE_NOT_ACCEPTABLE:
            LOG_ERROR << "Sending a raw notification failed. Too many notifications in to short of a time.";
            return ResponseCodeAction::ERROR;

        case StatusCodes::STATUS_CODE_GONE:
            LOG_ERROR << "Sending a raw notification failed. Channel URI expired.";
            return ResponseCodeAction::ERROR;

        case StatusCodes::STATUS_CODE_REQUEST_ENTRY_TOO_LARGE:
            LOG_ERROR << "Sending a raw notification failed. Payload > 5000 byte.";
            return ResponseCodeAction::ERROR;

        case StatusCodes::STATUS_CODE_INTERNAL_SERVER_ERROR:
            LOG_ERROR << "Sending a raw notification failed. Internal WNS server error.";
            return ResponseCodeAction::ERROR;

        case StatusCodes::STATUS_CODE_SERVICE_UNAVAILABLE:
            LOG_WARNING << "Sending a raw notification failed. WNS server currently unavailable.";
            return ResponseCodeAction::RETRY;
        default:
            LOG_ERROR << "Sending a raw notification failed. Status code: " << statusCode;
            LOG_ERROR << "Response: " << text;
            return ResponseCodeAction::ERROR;
    }
}

}  // namespace wns