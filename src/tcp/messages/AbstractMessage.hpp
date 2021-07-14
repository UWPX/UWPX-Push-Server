#pragma once

#include <nlohmann/json.hpp>
#include <string>

namespace tcp::messages {
class AbstractMessage {
 public:
    enum MessageType {
        ABSTRACT_MESSAGE,
        ABSTRACT_RESPONSE_MESSAGE,
        ERROR_RESPONSE_MESSAGE,
        REQUEST_TEST_PUSH_MESSAGE,
        SET_CHANNEL_URI_MESSAGE,
        SET_PUSH_ACCOUNT_MESSAGE,
        SUCCESS_RESPONSE_MESSAGE,
        SUCCESS_SET_PUSH_ACCOUNT_MESSAGE
    };

 private:
    static constexpr int VERSION = 1;

 protected:
    bool isValid;
    int version{-1};
    std::string action{};

 public:
    /**
     * Should be called when constructing a message after parsing it with the return value of from_json().
     **/
    explicit AbstractMessage(bool isValid);
    /**
     * Should be called when construction a message for sending it.
     **/
    explicit AbstractMessage(std::string&& action);
    AbstractMessage(AbstractMessage&&) = default;
    AbstractMessage(const AbstractMessage&) = default;
    AbstractMessage& operator=(AbstractMessage&&) = default;
    AbstractMessage& operator=(const AbstractMessage&) = default;
    virtual ~AbstractMessage() = default;

    [[nodiscard]] bool is_valid() const;
    [[nodiscard]] int get_version() const;
    [[nodiscard]] const std::string& get_action() const;
    [[nodiscard]] virtual MessageType get_type() const;

 protected:
    virtual bool from_json(const nlohmann::json& j);
    virtual void to_json(nlohmann::json& j) const;
};
}  // namespace tcp::messages