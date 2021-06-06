#include "WnsClient.hpp"
#include "logger/Logger.hpp"
#include <string>
#include <utility>
#include <cpr/api.h>
#include <cpr/cprtypes.h>
#include <cpr/payload.h>
#include <cpr/response.h>
#include <spdlog/spdlog.h>

namespace wns {
WnsClient::WnsClient(std::string&& packetSid, std::string&& clientSecret) : packetSid(std::move(packetSid)), clientSecret(std::move(clientSecret)) {}

bool WnsClient::isTokenValid() {
    return token && token->isValid();
}

bool WnsClient::requestToken() {
    cpr::Payload payload{
        {"grant_type", "client_credentials"},
        {"scope", "notify.windows.com"},
        {"client_id", packetSid},
        {"client_secret", clientSecret}};
    cpr::Url url{"https://login.live.com/accesstoken.srf"};
    cpr::Response response = cpr::Post(url, payload);
    if (response.status_code != 200) {
        SPDLOG_ERROR("Requesting a new token failed. Status code: {}\nResponse: {}", response.status_code, response.text);
        return false;
    }
    token = WnsToken::fromResponse(response.text);
    if (token) {
        SPDLOG_INFO("Successfully requested a new WNS token.");
        SPDLOG_DEBUG("Token: '{}', Type: '{}'", token->token, token->type);
        return true;
    }
    return false;
}

void WnsClient::loadTokenFromDb() {
}

bool WnsClient::sendRawNotification(const std::string& /*channelUri*/, const std::string&& /*content*/) {
    return true;
}

}  // namespace wns