#pragma once

#include "WnsToken.hpp"
#include "storage/ConfigurationStorage.hpp"
#include <memory>
#include <string>

namespace wns {
class WnsClient {
 private:
    std::string packetSid;
    std::string clientSecret;
    std::shared_ptr<WnsToken> token{nullptr};

 public:
    explicit WnsClient(const storage::WnsConfiguration& config);
    WnsClient(WnsClient&&) = default;
    WnsClient(const WnsClient&) = default;
    WnsClient& operator=(WnsClient&&) = default;
    WnsClient& operator=(const WnsClient&) = default;
    ~WnsClient() = default;

    bool isTokenValid();
    bool requestToken();
    void loadTokenFromDb();
    bool sendRawNotification(const std::string& channelUri, const std::string&& content);

 private:
};
}  // namespace wns