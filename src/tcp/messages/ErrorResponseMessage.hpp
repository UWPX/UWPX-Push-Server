#pragma once

#include "AbstractResponseMessage.hpp"
#include <nlohmann/json.hpp>
#include <string>

namespace tcp::messages {
class ErrorResponseMessage : public AbstractResponseMessage {
 private:
    static constexpr int STATUS = 0;

 protected:
    std::string error;

 public:
    explicit ErrorResponseMessage(const nlohmann::json& j);
    explicit ErrorResponseMessage(std::string&& error);
    ErrorResponseMessage(ErrorResponseMessage&&) = default;
    ErrorResponseMessage(const ErrorResponseMessage&) = default;
    ErrorResponseMessage& operator=(ErrorResponseMessage&&) = default;
    ErrorResponseMessage& operator=(const ErrorResponseMessage&) = default;
    ~ErrorResponseMessage() override = default;

    [[nodiscard]] const std::string& get_error() const;
    [[nodiscard]] MessageType get_type() const override;

 protected:
    bool from_json(const nlohmann::json& j) override;
    void to_json(nlohmann::json& j) const override;
};
}  // namespace tcp::messages