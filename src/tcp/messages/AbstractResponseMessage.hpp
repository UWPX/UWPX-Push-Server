#pragma once

#include "AbstractMessage.hpp"
#include <nlohmann/json.hpp>
#include <string_view>

namespace tcp::messages {
class AbstractResponseMessage : public AbstractMessage {
 private:
    static constexpr std::string_view ACTION = "response";

 protected:
    int status{-1};

 public:
    /**
     * Should be called when constructing a message after parsing it with the return value of from_json().
     **/
    explicit AbstractResponseMessage(bool isValid);
    /**
     * Should be called when construction a message for sending it.
     **/
    explicit AbstractResponseMessage(int status);
    AbstractResponseMessage(AbstractResponseMessage&&) = default;
    AbstractResponseMessage(const AbstractResponseMessage&) = default;
    AbstractResponseMessage& operator=(AbstractResponseMessage&&) = default;
    AbstractResponseMessage& operator=(const AbstractResponseMessage&) = default;
    ~AbstractResponseMessage() override;

    [[nodiscard]] int get_status() const;
    [[nodiscard]] MessageType get_type() const override;

 protected:
    bool from_json(const nlohmann::json& j) override;
    void to_json(nlohmann::json& j) const override;
};
}  // namespace tcp::messages