#pragma once

#include <chrono>
#include <cstddef>
#include <memory>
#include <string>

namespace wns {
class WnsToken {
 public:
    const std::string type;
    const std::string token;
    const std::chrono::system_clock::time_point expires;

    WnsToken(std::string&& type, std::string&& token, std::chrono::system_clock::time_point expires);
    WnsToken(WnsToken&&) = default;
    WnsToken(const WnsToken&) = default;
    WnsToken& operator=(WnsToken&&) = delete;
    WnsToken& operator=(const WnsToken&) = delete;
    ~WnsToken() = default;

    bool isValid();

    static std::shared_ptr<WnsToken> fromResponse(const std::string& response);

 private:
    static std::chrono::system_clock::time_point getExpiresDate(size_t expiresInSeconds);
};
}  // namespace wns