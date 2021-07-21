#pragma once

#include "AbstractResponseMessage.hpp"
#include <nlohmann/json.hpp>
#include <string>

namespace tcp::messages {
class SuccessResponseMessage : public AbstractResponseMessage {
 public:
    static constexpr int STATUS = 1;

    explicit SuccessResponseMessage(const nlohmann::json& j);
    SuccessResponseMessage();
    SuccessResponseMessage(SuccessResponseMessage&&) = default;
    SuccessResponseMessage(const SuccessResponseMessage&) = default;
    SuccessResponseMessage& operator=(SuccessResponseMessage&&) = default;
    SuccessResponseMessage& operator=(const SuccessResponseMessage&) = default;
    ~SuccessResponseMessage() override = default;

    [[nodiscard]] MessageType get_type() const override;
};
}  // namespace tcp::messages