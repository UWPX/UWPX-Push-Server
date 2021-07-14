#pragma once

#include "AbstractMessage.hpp"
#include <nlohmann/json.hpp>
#include <string>
#include <string_view>

namespace tcp::messages {
class RequestTestPushMessage : public AbstractMessage {
 public:
    static constexpr std::string_view ACTION = "request_test_push";

 protected:
    std::string deviceId;

 public:
    explicit RequestTestPushMessage(const nlohmann::json& j);
    explicit RequestTestPushMessage(std::string&& deviceId);
    RequestTestPushMessage(RequestTestPushMessage&&) = default;
    RequestTestPushMessage(const RequestTestPushMessage&) = default;
    RequestTestPushMessage& operator=(RequestTestPushMessage&&) = default;
    RequestTestPushMessage& operator=(const RequestTestPushMessage&) = default;
    ~RequestTestPushMessage() override = default;

    [[nodiscard]] const std::string& get_device_id() const;
    [[nodiscard]] MessageType get_type() const override;

 protected:
    bool from_json(const nlohmann::json& j) override;
    void to_json(nlohmann::json& j) const override;
};
}  // namespace tcp::messages