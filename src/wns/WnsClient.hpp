#pragma once

#include "WnsToken.hpp"
#include "storage/ConfigurationStorage.hpp"
#include "storage/redis/RedisClient.hpp"
#include <cstdint>
#include <memory>
#include <string>

namespace wns {
class WnsClient {
 private:
    /**
     * Response codes for WNS requests.
     * Source: https://docs.microsoft.com/en-us/previous-versions/windows/apps/hh465435(v=win.10)#response-codes
     **/
    enum StatusCodes : int {
        // The notification was accepted by WNS.
        STATUS_CODE_OK = 200,
        // One or more headers were specified incorrectly or conflict with another header.
        STATUS_CODE_BAD_REQUEST = 400,
        // The cloud service did not present a valid authentication ticket. The OAuth ticket may be invalid.
        STATUS_CODE_UNAUTHORIZED = 401,
        // The cloud service is not authorized to send a notification to this URI even though they are authenticated.
        STATUS_CODE_FORBIDDEN = 403,
        // The channel URI is not valid or is not recognized by WNS.
        STATUS_CODE_NOT_FOUND = 404,
        // Invalid method (GET, CREATE); only POST (Windows or Windows Phone) or DELETE (Windows Phone only) is allowed.
        STATUS_CODE_METHOD_NOT_ALLOWED = 405,
        // The cloud service exceeded its throttle limit.
        STATUS_CODE_NOT_ACCEPTABLE = 406,
        // The channel expired.
        STATUS_CODE_GONE = 410,
        // The notification payload exceeds the 5000 byte size limit.
        STATUS_CODE_REQUEST_ENTRY_TOO_LARGE = 413,
        // An internal failure caused notification delivery to fail.
        STATUS_CODE_INTERNAL_SERVER_ERROR = 500,
        // The server is currently unavailable.
        STATUS_CODE_SERVICE_UNAVAILABLE = 503
    };

    enum ResponseCodeAction {
        SUCCESS,
        RETRY,
        ERROR
    };

    std::string packetSid;
    std::string clientSecret;
    std::shared_ptr<WnsToken> token{nullptr};
    storage::redis::RedisClient* redisClient{nullptr};

 public:
    explicit WnsClient(const storage::WnsConfiguration& config);
    WnsClient(WnsClient&&) = default;
    WnsClient(const WnsClient&) = default;
    WnsClient& operator=(WnsClient&&) = default;
    WnsClient& operator=(const WnsClient&) = default;
    ~WnsClient() = default;

    /**
     * Returns true, in case a token exists and has not expired yet.
     **/
    bool is_token_valid();
    bool request_new_token();
    void load_token_from_db();
    bool send_raw_notification(const std::string& channelUri, const std::string&& content);
    void set_redis_client(storage::redis::RedisClient* redisClient);

 private:
    ResponseCodeAction handle_response_code(int64_t statusCode, const std::string& text);
    void store_token_in_db();
};
}  // namespace wns