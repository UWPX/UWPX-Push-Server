#pragma once

#include "AbstractMessage.hpp"
#include <nlohmann/json.hpp>
#include <string>
#include <string_view>

namespace tcp::messages {
class TestPushMessage : public AbstractMessage {
 public:
    static constexpr std::string_view ACTION = "test_push";

 public:
    explicit TestPushMessage(const nlohmann::json& j);
    TestPushMessage();
    TestPushMessage(TestPushMessage&&) = default;
    TestPushMessage(const TestPushMessage&) = default;
    TestPushMessage& operator=(TestPushMessage&&) = default;
    TestPushMessage& operator=(const TestPushMessage&) = default;
    ~TestPushMessage() override = default;

    [[nodiscard]] MessageType get_type() const override;
};
}  // namespace tcp::messages